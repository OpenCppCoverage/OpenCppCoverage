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
#include <boost/filesystem.hpp>

#include "tools/Log.hpp"

#include "CoverageData.hpp"
#include "Debugger.hpp"
#include "DebugInformation.hpp"
#include "ExecutedAddressManager.hpp"
#include "HandleInformation.hpp"
#include "BreakPoint.hpp"
#include "CoverageFilterManager.hpp"
#include "StartInfo.hpp"
#include "ExceptionHandler.hpp"
#include "CppCoverageException.hpp"
#include "Address.hpp"
#include "RunCoverageSettings.hpp"

#include "tools/Tool.hpp"

namespace CppCoverage
{			
	//-------------------------------------------------------------------------
	CodeCoverageRunner::CodeCoverageRunner()
	{ 
		executedAddressManager_.reset(new ExecutedAddressManager());
		exceptionHandler_.reset(new ExceptionHandler());
		breakpoint_.reset(new BreakPoint());
	}
	
	//-------------------------------------------------------------------------
	CodeCoverageRunner::~CodeCoverageRunner()
	{
	}

	//-------------------------------------------------------------------------
	CoverageData CodeCoverageRunner::RunCoverage(
		const RunCoverageSettings& settings)
	{
		Debugger debugger{ settings.GetCoverChildren(), settings.GetContinueAfterCppException()};

		coverageFilterManager_ = std::make_unique<CoverageFilterManager>(
			settings.GetCoverageFilterSettings(),
			settings.GetUnifiedDiffSettings(), 
			settings.GetOptimizedBuildSupport());
		const auto& startInfo = settings.GetStartInfo();
		int exitCode = debugger.Debug(startInfo, *this);
		const auto& path = startInfo.GetPath();

		auto warningMessageLines = coverageFilterManager_->ComputeWarningMessageLines(
			settings.GetMaxUnmatchPathsForWarning());
		for (const auto& line : warningMessageLines)
				LOG_WARNING << line;			

		return executedAddressManager_->CreateCoverageData(path.filename().wstring(), exitCode);
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO& processDebugInfo)
	{
		auto hProcess = processDebugInfo.hProcess;
		auto lpBaseOfImage = processDebugInfo.lpBaseOfImage;

		debugInformation_.emplace(hProcess, std::make_unique<DebugInformation>(hProcess));
		LoadModule(hProcess, processDebugInfo.hFile, lpBaseOfImage);
	}
	
	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnExitProcess(HANDLE hProcess, HANDLE, const EXIT_PROCESS_DEBUG_INFO&)
	{
		exceptionHandler_->OnExitProcess(hProcess);
		executedAddressManager_->OnExitProcess(hProcess);
		if (debugInformation_.erase(hProcess) != 1)
			THROW("Cannot find process for debugInformation_.");
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnLoadDll(
		HANDLE hProcess, 
		HANDLE hThread, 
		const LOAD_DLL_DEBUG_INFO& dllDebugInfo)
	{
		LoadModule(hProcess, dllDebugInfo.hFile, dllDebugInfo.lpBaseOfDll);
	}
	
	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnUnloadDll(
		HANDLE hProcess,
		HANDLE hThread,
		const UNLOAD_DLL_DEBUG_INFO& unloadDllDebugInfo)
	{
		executedAddressManager_->OnUnloadModule(hProcess, unloadDllDebugInfo.lpBaseOfDll);
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
	void CodeCoverageRunner::LoadModule(HANDLE hProcess, HANDLE hFile, void* baseOfImage)
	{
		HandleInformation handleInformation;

		std::wstring filename = handleInformation.ComputeFilename(hFile);
		
		if (coverageFilterManager_->IsModuleSelected(filename))
		{
			executedAddressManager_->AddModule(filename, baseOfImage);
			auto it = debugInformation_.find(hProcess);

			if (it == debugInformation_.end())
				THROW("Cannot find debug information.");
			const auto& debugInformation = it->second;

			debugInformation->LoadModule(filename, hFile, baseOfImage, *coverageFilterManager_, *this);
		}
	}
	
	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnNewLine(
		const std::wstring& filename, 
		int lineNumber, 
		const Address& address)
	{		
		auto oldInstruction = breakpoint_->SetBreakPointAt(address);

		if (!executedAddressManager_->RegisterAddress(address, filename, lineNumber, oldInstruction))
			breakpoint_->RemoveBreakPoint(address, oldInstruction);
	}

	//-------------------------------------------------------------------------
	size_t CodeCoverageRunner::GetDebugInformationCount() const
	{
		return debugInformation_.size();
	}
}
