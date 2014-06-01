#include "stdafx.h"
#include "Tool.hpp"

#include <cvt/wstring>
#include <iostream>
#include <codecvt>
#include <DbgHelp.h>

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
			std::vector<wchar_t> filename(4096);

			if (!GetModuleFileName(nullptr, &filename[0], filename.size()))
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
	}

	//-------------------------------------------------------------------------
	std::string ToString(const std::wstring& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

		return converter.to_bytes(str);
	}

	//-------------------------------------------------------------------------
	std::wstring ToWString(const std::string& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		
		return converter.from_bytes(str);
	}
		
	//-------------------------------------------------------------------------
	boost::filesystem::path GetTemplateFolder()
	{
		return GetExecutableFolder() / "Template";
	}
		
	//-------------------------------------------------------------------------
	boost::filesystem::path GetUniquePath(const boost::filesystem::path& prefix)
	{
		std::string uniquePath = prefix.string();

		for (int i = 2; fs::exists(uniquePath); ++i)
			uniquePath = prefix.string() + '-' + std::to_string(i);

		return uniquePath;
	}

	//-------------------------------------------------------------------------
	void Try(std::function<void()> action)
	{
		try
		{
			action();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "ERROR: " << e.what();
		}
		catch (...)
		{
			LOG_ERROR << "Unkown exception";
		}
	}

	//-------------------------------------------------------------------------
	void CheckPathExists(const fs::path& path)
	{
		if (!fs::exists(path))
			THROW(path.wstring() << L" does not exist");
	}

	//-------------------------------------------------------------------------
	void CreateMiniDumpOnUnHandledException()
	{
		DWORD dwMode = GetErrorMode();
		SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);

		SetUnhandledExceptionFilter(CreateMiniDumpOnUnHandledException);
	}
}