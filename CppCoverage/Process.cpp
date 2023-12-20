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
#include "Process.hpp"

#include <Windows.h>
#include <boost/optional.hpp>

#include "Tools/Log.hpp"
#include "Tools/Tool.hpp"

#include "StartInfo.hpp"
#include "CppCoverageException.hpp"

namespace CppCoverage
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

	const std::wstring Process::CannotFindPathMessage = L"Cannot find path: ";
	const std::wstring Process::CheckIfValidExecutableMessage =
	    L"Cannot run process, check if it is a valid executable:";

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
			auto hProcess = processInformation_->hProcess;
			if (hProcess && !CloseHandle(hProcess))
				LOG_ERROR << "Cannot close process handle";

			auto hThread = processInformation_->hThread;
			if (hThread && !CloseHandle(hThread))
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

		processInformation_ = PROCESS_INFORMATION{};
		if (!CreateProcess(
			nullptr,
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
			std::wostringstream ostr;

			if (!Tools::FileExists(startInfo_.GetPath()))
				ostr << CannotFindPathMessage + startInfo_.GetPath().wstring();
			else
			{
				ostr
				    << CheckIfValidExecutableMessage
				    << std::endl;

#ifndef _WIN64
				ostr << L"\n*** This version support only 32 bits executable "
				        L"***.\n\n";
#endif
				ostr << startInfo_
				     << CppCoverage::GetErrorMessage(GetLastError());
			}
			throw std::runtime_error(Tools::ToLocalString(ostr.str()));
		}		
	}
}
