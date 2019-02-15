// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
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
#include "Tool.hpp"

#include <boost/optional/optional.hpp>
#include <cvt/wstring>
#include <iostream>
#include <codecvt>

#include "DbgHelp.hpp"
#include "Log.hpp"
#include "ToolsException.hpp"
#include "ScopedAction.hpp"

namespace fs = boost::filesystem;

namespace Tools
{
	namespace
	{
		//-----------------------------------------------------------------------------
		fs::path GetExecutablePath()
		{
			const int PathBufferSize = 40 * 1000;
			std::vector<wchar_t> filename(PathBufferSize);

			if (!GetModuleFileName(nullptr, &filename[0], static_cast<int>(filename.size())))
				THROW("Cannot get current executable path.");

			return fs::path{ &filename[0] };
		}

		//-----------------------------------------------------------------------------
		fs::path GetExecutableFolder()
		{
			fs::path executablePath = GetExecutablePath();

			return executablePath.parent_path();
		}

		//-----------------------------------------------------------------------------
		MINIDUMP_TYPE GetMiniDumpDefaultType()
		{
			return static_cast<MINIDUMP_TYPE>(MiniDumpWithDataSegs |
				MiniDumpWithPrivateReadWriteMemory |
				MiniDumpWithFullMemoryInfo |
				MiniDumpWithThreadInfo);
		}

		//-----------------------------------------------------------------------------
		void CreateMiniDump(
			MINIDUMP_EXCEPTION_INFORMATION& minidumpInfo, 
			HANDLE hFile, 			
			const wchar_t* dmpFilename)
		{
			auto miniDumpType = GetMiniDumpDefaultType();

			std::wcerr << L"\tTrying to create memory dump..." << std::endl;
			if (MiniDumpWriteDump(
				GetCurrentProcess(),
				GetCurrentProcessId(),
				hFile,
				miniDumpType,
				&minidumpInfo,
				nullptr,
				nullptr))
			{
				std::wcerr << "\tMemory dump created successfully: " << dmpFilename << std::endl;
				std::wcerr << "\tPlease create a new issue on ";
				std::wcerr << "https://opencppcoverage.codeplex.com/workitem/list/basic and attached the memory dump ";
				std::wcerr << dmpFilename << std::endl;
			}
			else
				std::cerr << "\tFailed to create memory dump." << std::endl;
		}
		
		//-----------------------------------------------------------------------------
		LONG WINAPI CreateMiniDumpOnUnHandledException(PEXCEPTION_POINTERS exceptionInfo)
		{
			MINIDUMP_EXCEPTION_INFORMATION minidumpInfo;

			std::wcerr << L"Unexpected error occurs." << std::endl;			

			minidumpInfo.ThreadId = GetCurrentThreadId();
			minidumpInfo.ExceptionPointers = exceptionInfo;
			minidumpInfo.ClientPointers = FALSE;

			const auto dmpFilename = L"OpenCppCoverage.dmp";
			HANDLE hFile = CreateFile(dmpFilename, GENERIC_WRITE, 0, nullptr,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			
			if (hFile != INVALID_HANDLE_VALUE)
			{
				ScopedAction fileCloser{ [hFile](){ CloseHandle(hFile); } };
				CreateMiniDump(minidumpInfo, hFile, dmpFilename);
			}
			abort();
			return 0;
		}

		//-------------------------------------------------------------------------
		std::string ToString(unsigned int pageCode, const std::wstring& str)
		{
			if (str.empty())
				return{};

			auto size = WideCharToMultiByte(pageCode, 0, str.c_str(),
				static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
			std::vector<char> buffer(size);

			if (!WideCharToMultiByte(pageCode, 0, str.c_str(), static_cast<int>(str.size()),
				&buffer[0], static_cast<int>(buffer.size()), nullptr, nullptr))
			{
				throw std::runtime_error("Error in WideCharToMultiByte.");
			}

			return{ buffer.begin(), buffer.end() };
		}

		//-------------------------------------------------------------------------
		std::wstring ToWString(unsigned int pageCode, const std::string& str)
		{
			if (str.empty())
				return{};

			auto size = MultiByteToWideChar(pageCode, 0,
				str.c_str(), static_cast<int>(str.size()), nullptr, 0);
			std::vector<wchar_t> buffer(size);

			if (!MultiByteToWideChar(pageCode, 0, str.c_str(), static_cast<int>(str.size()),
				&buffer[0], static_cast<int>(buffer.size())))
			{
				throw std::runtime_error("Error in MultiByteToWideChar for " + str);
			}

			return{ buffer.begin(), buffer.end() };
		}
	}

	//-------------------------------------------------------------------------
	std::string ToLocalString(const std::wstring& str)
	{
		return ToString(CP_ACP, str);
	}

	//-------------------------------------------------------------------------
	std::string ToUtf8String(const std::wstring& str)
	{
		return ToString(CP_UTF8, str);
	}

	//-------------------------------------------------------------------------
	std::wstring LocalToWString(const std::string& str)
	{
		return ToWString(CP_ACP, str);
	}

	//-------------------------------------------------------------------------
	std::wstring Utf8ToWString(const std::string& str)
	{
		return ToWString(CP_UTF8, str);
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path GetTemplateFolder()
	{
		return GetExecutableFolder() / "Template";
	}

	//-------------------------------------------------------------------------
	boost::optional<std::wstring> Try(std::function<void()> action)
	{
		try
		{
			action();
		}
		catch (const std::exception& e)
		{
			return Tools::LocalToWString(e.what());
		}
		catch (...)
		{
			return std::wstring{L"Unkown exception"};
		}

		return boost::none;
	}

	//-------------------------------------------------------------------------
	void CreateMiniDumpOnUnHandledException()
	{
		DWORD dwMode = GetErrorMode();
		SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);

		SetUnhandledExceptionFilter(CreateMiniDumpOnUnHandledException);
	}

	//-------------------------------------------------------------------------
	void ShowOutputMessage(
		const std::wstring& message, 
		const boost::filesystem::path& path)
	{
		LOG_INFO << GetSeparatorLine();
		LOG_INFO << message << path.wstring();
		LOG_INFO << GetSeparatorLine();
	}

	//-------------------------------------------------------------------------
	std::wstring GetSeparatorLine()
	{
		return L"----------------------------------------------------";
	}
	
	//---------------------------------------------------------------------
	void CreateParentFolderIfNeeded(const boost::filesystem::path& path)
	{
		if (path.has_parent_path() && !path.filename_is_dot())
		{
			auto parentPath = path.parent_path();
			boost::system::error_code er;

			boost::filesystem::create_directories(parentPath, er);
			if (er)
			{
				THROW(L"Error when creating folder " << parentPath.wstring()
					<< L" Error code:" << er.value());
			}
		}
	}
}