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
#include "CodeCoverageRunner.hpp"

#include <sstream>
#include <boost/optional.hpp>

#include "tools/Log.hpp"

#include "Plugin/Exporter/CoverageData.hpp"
#include "Debugger.hpp"
#include "ExecutedAddressManager.hpp"
#include "HandleInformation.hpp"
#include "BreakPoint.hpp"
#include "CoverageFilterManager.hpp"
#include "StartInfo.hpp"
#include "ExceptionHandler.hpp"
#include "CppCoverageException.hpp"
#include "Address.hpp"
#include "RunCoverageSettings.hpp"
#include "MonitoredLineRegister.hpp"
#include "FilterAssistant.hpp"
#include "FileSystem.hpp"

#include "Tools/WarningManager.hpp"
#include "Tools/Tool.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	CodeCoverageRunner::CodeCoverageRunner(
	    std::shared_ptr<Tools::WarningManager> warningManager)
	    : warningManager_{warningManager},
	      filterAssistant_{
	          std::make_shared<FilterAssistant>(std::make_shared<FileSystem>())}
	{
		executedAddressManager_ = std::make_shared<ExecutedAddressManager>();
		exceptionHandler_ = std::make_unique<ExceptionHandler>();
		breakpoint_ = std::make_shared<BreakPoint>();
	}
	
	//-------------------------------------------------------------------------
	CodeCoverageRunner::~CodeCoverageRunner()
	{
	}

	//-------------------------------------------------------------------------
	Plugin::CoverageData CodeCoverageRunner::RunCoverage(
		const RunCoverageSettings& settings)
	{
		Debugger debugger{ settings.GetCoverChildren(), settings.GetContinueAfterCppException(), settings.GetStopOnAssert()};

		coverageFilterManager_ = std::make_shared<CoverageFilterManager>(
			settings.GetCoverageFilterSettings(),
			settings.GetUnifiedDiffSettings(), 
			settings.GetExcludedLineRegexes(),
			settings.GetOptimizedBuildSupport());

		monitoredLineRegister_ = std::make_unique<MonitoredLineRegister>(
		    breakpoint_,
		    executedAddressManager_,
		    coverageFilterManager_,
		    std::make_unique<DebugInformationEnumerator>(settings.GetSubstitutePdbSourcePaths()),
			filterAssistant_);

		const auto& startInfo = settings.GetStartInfo();
		int exitCode = debugger.Debug(startInfo, *this);
		const auto& path = startInfo.GetPath();

		auto warningMessageLines = coverageFilterManager_->ComputeWarningMessageLines(
			settings.GetMaxUnmatchPathsForWarning());
		for (const auto& line : warningMessageLines)
				LOG_WARNING << line;
		auto filterAdviceMessage = filterAssistant_->GetAdviceMessage();
		if (filterAdviceMessage)
			warningManager_->AddWarning(*filterAdviceMessage);
		return executedAddressManager_->CreateCoverageData(path.filename().wstring(), exitCode);
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnCreateProcess(HANDLE hProcess, const wchar_t* pszImageName, void* lpBaseOfImage)
	{
		LoadModule(hProcess, pszImageName, lpBaseOfImage);
	}
	
	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnExitProcess(HANDLE hProcess)
	{
		exceptionHandler_->OnExitProcess(hProcess);
		executedAddressManager_->OnExitProcess(hProcess);
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnLoadDll(HANDLE hProcess, const wchar_t* pszImageName, void* lpBaseOfImage)
	{
		LoadModule(hProcess, pszImageName, lpBaseOfImage);
	}
	
	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnUnloadDll(HANDLE hProcess, void* lpBaseOfImage)
	{
		executedAddressManager_->OnUnloadModule(hProcess, lpBaseOfImage);
	}

	//-------------------------------------------------------------------------
	IDebugEventsHandler::ExceptionType CodeCoverageRunner::OnException(
		HANDLE hProcess, 
		HANDLE hThread, 
		const EXCEPTION_DEBUG_INFO& exceptionDebugInfo)
	{
		std::wostringstream ostr;
		
		auto status = exceptionHandler_->HandleException(hProcess, exceptionDebugInfo, ostr);

		switch (status)
		{
			case CppCoverage::ExceptionHandlerStatus::BreakPoint:
			{
				if (OnBreakPoint(exceptionDebugInfo, hProcess, hThread))
					return IDebugEventsHandler::ExceptionType::BreakPoint;
				return IDebugEventsHandler::ExceptionType::InvalidBreakPoint;
			}
			case CppCoverage::ExceptionHandlerStatus::FirstChanceException:
			{
				return IDebugEventsHandler::ExceptionType::NotHandled;
			}
			case CppCoverage::ExceptionHandlerStatus::Error:
			{
				LOG_ERROR << ostr.str();
				
				return IDebugEventsHandler::ExceptionType::Error;
			}
			case CppCoverage::ExceptionHandlerStatus::CppError:
			{
				LOG_ERROR << ostr.str();

				return IDebugEventsHandler::ExceptionType::CppError;
			}
		}

		return IDebugEventsHandler::ExceptionType::NotHandled;
	}
	
	//-------------------------------------------------------------------------
	bool CodeCoverageRunner::OnBreakPoint(
		const EXCEPTION_DEBUG_INFO& exceptionDebugInfo,
		HANDLE hProcess,
		HANDLE hThread)
	{
		const auto& exceptionRecord = exceptionDebugInfo.ExceptionRecord;
		auto addressValue = exceptionRecord.ExceptionAddress;
		Address address{ hProcess, addressValue };
		auto oldInstruction = executedAddressManager_->MarkAddressAsExecuted(address);

		if (oldInstruction)
		{
			breakpoint_->RemoveBreakPoint(address, *oldInstruction);
			breakpoint_->AdjustEipAfterBreakPointRemoval(hThread);
			return true;
		}

		return false;
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::LoadModule(HANDLE hProcess,const wchar_t* pszImageName,void* baseOfImage)
	{
		auto isSelected = coverageFilterManager_->IsModuleSelected(pszImageName);
		if (isSelected) {
			isSelected = monitoredLineRegister_->RegisterLineToMonitor(pszImageName, hProcess, baseOfImage);
		}
		filterAssistant_->OnNewModule(pszImageName, isSelected);
	}
}
