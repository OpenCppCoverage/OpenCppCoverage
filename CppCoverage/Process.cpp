#include "stdafx.h"
#include "Process.hpp"

#include <Windows.h>
#include <boost/optional.hpp>
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
				commandLine = std::vector<wchar_t>();
				std::vector<wchar_t>& buffer = commandLine.get();
				for (const auto& argument : arguments)
					buffer.insert(buffer.end(), argument.begin(), argument.end());
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
			CloseHandle(processInformation_->hProcess); // $$ log if error
			CloseHandle(processInformation_->hThread);
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

		processInformation_ = PROCESS_INFORMATION();
		if (!CreateProcess(
			startInfo_.GetFilename().c_str(),
			(optionalCommandLine) ? &(*optionalCommandLine)[0] : nullptr,
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
