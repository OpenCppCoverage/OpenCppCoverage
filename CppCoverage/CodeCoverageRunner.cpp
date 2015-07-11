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
#include "CoverageFilter.hpp"
#include "StartInfo.hpp"
#include "ExceptionHandler.hpp"
#include "CppCoverageException.hpp"
#include "Address.hpp"

#include "tools/Tool.hpp"

namespace CppCoverage
{			
	//-------------------------------------------------------------------------
	CodeCoverageRunner::CodeCoverageRunner()
		: unhandledBreakPoint_{ false }
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
	CoverageData CodeCoverageRunner::RunCoverage(const StartInfo& startInfo, const CoverageSettings& coverageSettings)
	{
		Debugger debugger;

		coverageFilter_.reset(new CoverageFilter(coverageSettings));
		int exitCode = debugger.Debug(startInfo, *this);

		if (exitCode == 0 && unhandledBreakPoint_)
			exitCode = EXCEPTION_BREAKPOINT;

		const auto& path = startInfo.GetPath();

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
	void CodeCoverageRunner::OnLoadDll(
		HANDLE hProcess, 
		HANDLE hThread, 
		const LOAD_DLL_DEBUG_INFO& dllDebugInfo)
	{
		LoadModule(hProcess, dllDebugInfo.hFile, dllDebugInfo.lpBaseOfDll);
	}
	
	//-------------------------------------------------------------------------
	DWORD CodeCoverageRunner::OnException(
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
				OnBreakPoint(exceptionDebugInfo, hProcess, hThread);
				return DBG_CONTINUE;
			}
			case CppCoverage::ExceptionHandlerStatus::FirstChanceException:
			{
				return DBG_EXCEPTION_NOT_HANDLED;
			}
			case CppCoverage::ExceptionHandlerStatus::Fatal:
			{
				LOG_ERROR << ostr.str();
				
				return DBG_EXCEPTION_NOT_HANDLED;
			}			
		}

		return DBG_EXCEPTION_NOT_HANDLED;
	}
	
	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnBreakPoint(
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
		}
		else
		{
			LOG_WARNING << "--------------------------------------------------------------------------------";
			LOG_WARNING << "It seems there is an assertion failure or you call DebugBreak() in your program.";
			LOG_WARNING << "--------------------------------------------------------------------------------";
			unhandledBreakPoint_ = true;
		}
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::LoadModule(HANDLE hProcess, HANDLE hFile, void* baseOfImage)
	{
		HandleInformation handleInformation;

		std::wstring filename = handleInformation.ComputeFilename(hFile);
		
		if (coverageFilter_->IsModuleSelected(filename))
		{
			executedAddressManager_->SetCurrentModule(filename);			
			auto it = debugInformation_.find(hProcess);

			if (it == debugInformation_.end())
				THROW("Cannot find debug information.");
			const auto& debugInformation = it->second;

			debugInformation->LoadModule(filename, hFile, baseOfImage, *this);
		}
	}

	//-------------------------------------------------------------------------
	bool CodeCoverageRunner::IsSourceFileSelected(const std::wstring& filename) const
	{		
		return coverageFilter_->IsSourceFileSelected(filename);
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
}
