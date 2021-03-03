// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2020 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "MiniDump.hpp"

#include <iostream>

#pragma warning(push)
#pragma warning(disable: 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <DbgHelp.h>
#pragma warning(pop)

namespace Tools
{
	namespace
	{

		//-----------------------------------------------------------------------------
		MINIDUMP_TYPE GetMiniDumpDefaultType()
		{
			return static_cast<MINIDUMP_TYPE>(MiniDumpWithDataSegs |
				MiniDumpWithPrivateReadWriteMemory |
				MiniDumpWithFullMemoryInfo |
				MiniDumpWithThreadInfo);
		}

		//-----------------------------------------------------------------------------
		bool CreateMiniDump(
			PEXCEPTION_POINTERS exceptionInfo,
			DWORD dwProcessId,
			DWORD dwThreadId,
			HANDLE hProcess,
			const wchar_t* dmpFilename,
			bool isOpenCppCrash)
		{
			MINIDUMP_EXCEPTION_INFORMATION minidumpInfo;
			auto miniDumpType = GetMiniDumpDefaultType();
			HANDLE hFile = CreateFileW(dmpFilename, GENERIC_WRITE, 0, nullptr,
									   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (hFile == INVALID_HANDLE_VALUE)
				return false;

			minidumpInfo.ThreadId = dwThreadId;
			minidumpInfo.ExceptionPointers = exceptionInfo;
			minidumpInfo.ClientPointers = FALSE;

			std::wcerr << L"\tTrying to create memory dump..." << std::endl;

			bool succeeded = false;
			// Loop from https://github.com/dotnet/diagnostics/blob/master/src/Tools/dotnet-dump/Dumper.Windows.cs
			for (int n = 0; n < 5; ++n)
			{
				if (MiniDumpWriteDump(
					hProcess,
					dwProcessId,
					hFile,
					miniDumpType,
					&minidumpInfo,
					nullptr,
					nullptr))
				{
					succeeded = true;
					break;
				}
				else
				{
					if (GetLastError() != HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY))
						break;
				}
			}

			if (succeeded)
			{
				if (isOpenCppCrash)
				{
					std::wcerr << "\tMemory dump created successfully: " << dmpFilename << std::endl;
					std::wcerr << "\tPlease create a new issue on ";
					std::wcerr << "https://github.com/OpenCppCoverage/OpenCppCoverage/issues and attached the memory dump ";
					std::wcerr << dmpFilename << std::endl;
				}
			}
			else
			{
				if (isOpenCppCrash)
					std::cerr << "\tFailed to create memory dump." << std::endl;
			}

			CloseHandle(hFile);
			return succeeded;
		}
		
		//-----------------------------------------------------------------------------
		LONG WINAPI CreateMiniDumpOnUnHandledException(PEXCEPTION_POINTERS exceptionInfo)
		{
			std::wcerr << L"Unexpected error occurs." << std::endl;

			const auto dmpFilename = L"OpenCppCoverage.dmp";

			CreateMiniDump(exceptionInfo,
						   GetCurrentProcessId(),
						   GetCurrentThreadId(),
						   GetCurrentProcess(),
						   dmpFilename,
						   true);

			abort();
			return 0;
		}
	}

	//-------------------------------------------------------------------------
	void CreateMiniDumpOnUnHandledException()
	{
		DWORD dwMode = GetErrorMode();
		SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);

		SetUnhandledExceptionFilter(CreateMiniDumpOnUnHandledException);
	}	

	bool CreateMiniDumpFromException(PEXCEPTION_POINTERS exceptionInfo,
									 DWORD dwProcessId,
									 DWORD dwThreadId,
									 HANDLE hProcess,
									 const wchar_t* dmpFilename)
	{
		return CreateMiniDump(exceptionInfo, dwProcessId, dwThreadId, hProcess, dmpFilename, false);
	}
}
