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
#include "Debugger.hpp"

#include "tools/Log.hpp"
#include "tools/ScopedAction.hpp"

#include "Process.hpp"
#include "CppCoverageException.hpp"
#include "IDebugEventsHandler.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	namespace
	{
		void OnRip(const RIP_INFO& ripInfo)
		{
			LOG_ERROR << "Debugee process terminate unexpectedly:"
				<< "(type:" << ripInfo.dwType << ")"
				<< GetErrorMessage(ripInfo.dwError);
		}
	}	

	//-------------------------------------------------------------------------
	struct Debugger::ProcessStatus
	{
		ProcessStatus() = default;

		ProcessStatus(
			boost::optional<int> exitCode,
			boost::optional<DWORD> continueStatus)
			: exitCode_{ exitCode }
			, continueStatus_{ continueStatus }
		{
		}

		boost::optional<int> exitCode_;
		boost::optional<DWORD> continueStatus_;
	};

	//-------------------------------------------------------------------------
	int Debugger::Debug(const StartInfo& startInfo, IDebugEventsHandler& debugEventsHandler)
	{
		Process process(startInfo);
		process.Start(DEBUG_PROCESS);
		
		DEBUG_EVENT debugEvent;
		boost::optional<int> exitCode;

		processHandles_.clear();
		threadHandles_.clear();
		rootProcessId_ = boost::none;

		while (!exitCode || !processHandles_.empty())
		{
			if (!WaitForDebugEvent(&debugEvent, INFINITE))
				THROW_LAST_ERROR(L"Error WaitForDebugEvent:", GetLastError());

			ProcessStatus processStatus = HandleDebugEvent(debugEvent, debugEventsHandler);
			
			// Get the exit code of the root process
			if (processStatus.exitCode_ && rootProcessId_ == debugEvent.dwProcessId)
				exitCode = processStatus.exitCode_;

			auto continueStatus = boost::get_optional_value_or(processStatus.continueStatus_, DBG_CONTINUE);

			if (!ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, continueStatus))
				THROW_LAST_ERROR("Error in ContinueDebugEvent:", GetLastError());
		}

		return *exitCode;
	}
	
	//-------------------------------------------------------------------------
	Debugger::ProcessStatus Debugger::HandleDebugEvent(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler)
	{
		auto dwProcessId = debugEvent.dwProcessId;
		auto dwThreadId = debugEvent.dwThreadId;

		switch (debugEvent.dwDebugEventCode)
		{
			case CREATE_PROCESS_DEBUG_EVENT: OnCreateProcess(debugEvent, debugEventsHandler); break;
			case CREATE_THREAD_DEBUG_EVENT: OnCreateThread(debugEvent.u.CreateThread.hThread, dwThreadId); break;
			default:
			{
				auto hProcess = GetProcessHandle(dwProcessId);
				auto hThread = GetThreadHandle(dwThreadId);
				return HandleNotCreationalEvent(debugEvent, debugEventsHandler, hProcess, hThread, dwThreadId);
			}
		}

		return{};
	}


	//-------------------------------------------------------------------------
	Debugger::ProcessStatus
		Debugger::HandleNotCreationalEvent(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler,
		HANDLE hProcess,
		HANDLE hThread,
		DWORD dwThreadId)
	{
		switch (debugEvent.dwDebugEventCode)
		{
			case EXIT_PROCESS_DEBUG_EVENT:
			{
				auto exitCode = OnExitProcess(debugEvent, hProcess, hThread, debugEventsHandler);
				return ProcessStatus{exitCode, boost::none};
			}
			case EXIT_THREAD_DEBUG_EVENT: OnExitThread(dwThreadId); break;
			case LOAD_DLL_DEBUG_EVENT:
			{
				const auto& loadDll = debugEvent.u.LoadDll;
				Tools::ScopedAction scopedAction{ [&loadDll]{ CloseHandle(loadDll.hFile); } };
				debugEventsHandler.OnLoadDll(hProcess, hThread, loadDll);
				break;
			}
			case EXCEPTION_DEBUG_EVENT:
			{
				auto continueStatus = debugEventsHandler.OnException(hProcess, hThread, debugEvent.u.Exception);
				return ProcessStatus{ boost::none, continueStatus };
			}
			case RIP_EVENT: OnRip(debugEvent.u.RipInfo); break;
			default: LOG_DEBUG << "Debug event:" << debugEvent.dwDebugEventCode; break;
		}

		return ProcessStatus{};
	}
	
	//-------------------------------------------------------------------------
	void Debugger::OnCreateProcess(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler)
	{		
		const auto& processInfo = debugEvent.u.CreateProcessInfo;
		Tools::ScopedAction scopedAction{ [&processInfo]{ CloseHandle(processInfo.hFile); } };

		LOG_DEBUG << "Create new Process:" << debugEvent.dwProcessId;

		if (!rootProcessId_ && processHandles_.empty())
			rootProcessId_ = debugEvent.dwProcessId;

		if (!processHandles_.emplace(debugEvent.dwProcessId, processInfo.hProcess).second)
			THROW("Process id already exist");
				
		debugEventsHandler.OnCreateProcess(processInfo);

		OnCreateThread(processInfo.hThread, debugEvent.dwThreadId);
	}

	//-------------------------------------------------------------------------
	int Debugger::OnExitProcess(
		const DEBUG_EVENT& debugEvent,
		HANDLE hProcess,
		HANDLE hThread,
		IDebugEventsHandler& debugEventsHandler)
	{
		LOG_DEBUG << "Exit Process:" << hProcess;

		auto exitProcess = debugEvent.u.ExitProcess;
		debugEventsHandler.OnExitProcess(hProcess, hThread, exitProcess);

		if (processHandles_.erase(debugEvent.dwProcessId) != 1)
			THROW("Cannot find exited process.");

		return exitProcess.dwExitCode;
	}

	//-------------------------------------------------------------------------
	void Debugger::OnCreateThread(
		HANDLE hThread,
		DWORD dwThreadId)
	{
		LOG_DEBUG << "Create Process:" << dwThreadId;

		if (!threadHandles_.emplace(dwThreadId, hThread).second)
			THROW("Thread id already exist");
	}
	
	//-------------------------------------------------------------------------
	void Debugger::OnExitThread(DWORD dwThreadId)
	{	
		LOG_DEBUG << "Exit thread:" << dwThreadId;

		if (threadHandles_.erase(dwThreadId) != 1)
			THROW("Cannot find exited thread.");
	}

	//-------------------------------------------------------------------------
	HANDLE Debugger::GetProcessHandle(DWORD dwProcessId) const
	{
		return processHandles_.at(dwProcessId);
	}

	//-------------------------------------------------------------------------
	HANDLE Debugger::GetThreadHandle(DWORD dwThreadId) const
	{
		return threadHandles_.at(dwThreadId);
	}

}