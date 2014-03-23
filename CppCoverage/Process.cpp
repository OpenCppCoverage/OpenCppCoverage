#include "stdafx.h"
#include "Process.hpp"

#include <Windows.h>
#include <boost/optional.hpp>

#include "tools/Log.hpp"

#include "StartInfo.hpp"
#include "CppCoverageException.hpp"

namespace CppCoverage
{
	namespace
	{
		//---------------------------------------------------------------------
		boost::optional<std::vector<wchar_t>> 
			CreateCommandLine(const std::vector<std::wstring>& arguments)
		{
			boost::optional<std::vector<wchar_t>> commandLine;

			if (!arguments.empty())
			{				
				std::vector<wchar_t> buffer;
				for (const auto& argument : arguments)
				{
					buffer.push_back(L'\"');
					buffer.insert(buffer.end(), argument.begin(), argument.end());
					buffer.push_back(L'\"');
					buffer.push_back(L' ');
				}
					
				buffer.push_back(L'\0');
				return buffer;
			}

			return commandLine;
		}		
	}

	//-------------------------------------------------------------------------
	Process::Process(const StartInfo& startInfo)
		: startInfo_(startInfo)
	{		
	}

	//-------------------------------------------------------------------------
	Process::~Process()
	{
		if (processInformation_)
		{			
			if (!CloseHandle(processInformation_->hProcess))
				LOG_ERROR << "Cannot close process handle";

			if (!CloseHandle(processInformation_->hThread))
				LOG_ERROR << "Cannot close thread handle";
		}
	}

	//-------------------------------------------------------------------------
	void Process::Start(DWORD creationFlags)
	{
		if (processInformation_)
			THROW(L"Process already started");

		STARTUPINFO lpStartupInfo;

		ZeroMemory(&lpStartupInfo, sizeof(lpStartupInfo));
		const auto* workindDirectory = startInfo_.GetWorkingDirectory();
		auto optionalCommandLine = CreateCommandLine(startInfo_.GetArguments());
		auto commandLine = (optionalCommandLine) ? &(*optionalCommandLine)[0] : nullptr;

		processInformation_ = PROCESS_INFORMATION();
		if (!CreateProcess(
			startInfo_.GetPath().c_str(),
			commandLine,
			nullptr,
			nullptr,
			FALSE,
			creationFlags,
			nullptr,
			(workindDirectory) ? workindDirectory->c_str() : nullptr,
			&lpStartupInfo,
			&processInformation_.get()
			))
		{
			THROW_LAST_ERROR(L"Error when creating the process:" << startInfo_, GetLastError());
		}		
	}
}
