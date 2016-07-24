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

#include "Tools/Tool.hpp"

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
	Debugger::Debugger(
		bool coverChildren,
		bool continueAfterCppException)
		: coverChildren_{ coverChildren }
		, continueAfterCppException_{ continueAfterCppException }
	{
	}

	//-------------------------------------------------------------------------
	int Debugger::Debug(
		const StartInfo& startInfo,
		IDebugEventsHandler& debugEventsHandler)
	{
		Process process(startInfo);
		process.Start((coverChildren_) ? DEBUG_PROCESS: DEBUG_ONLY_THIS_PROCESS);
		
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
			// Set once as we do not want EXCEPTION_BREAKPOINT to be override
			if (processStatus.exitCode_ && rootProcessId_ == debugEvent.dwProcessId && !exitCode)
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
			case UNLOAD_DLL_DEBUG_EVENT:
			{
				debugEventsHandler.OnUnloadDll(hProcess, hThread, debugEvent.u.UnloadDll);
				break;
			}
			case EXCEPTION_DEBUG_EVENT: return OnException(debugEvent, debugEventsHandler, hProcess, hThread);
			case RIP_EVENT: OnRip(debugEvent.u.RipInfo); break;
			default: LOG_DEBUG << "Debug event:" << debugEvent.dwDebugEventCode; break;
		}

		return ProcessStatus{};
	}
	
	//-------------------------------------------------------------------------
	Debugger::ProcessStatus
		Debugger::OnException(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler,
		HANDLE hProcess,
		HANDLE hThread) const
	{
		const auto& exception = debugEvent.u.Exception;
		auto exceptionType = debugEventsHandler.OnException(hProcess, hThread, exception);

		switch (exceptionType)
		{
			case IDebugEventsHandler::ExceptionType::BreakPoint:
			{
				return ProcessStatus{ boost::none, DBG_CONTINUE };
			}
			case IDebugEventsHandler::ExceptionType::InvalidBreakPoint:
			{
				LOG_WARNING << Tools::GetSeparatorLine();
				LOG_WARNING << "It seems there is an assertion failure or you call DebugBreak() in your program.";
				LOG_WARNING << Tools::GetSeparatorLine();

				return ProcessStatus( EXCEPTION_BREAKPOINT, DBG_CONTINUE );
			}
			case IDebugEventsHandler::ExceptionType::NotHandled:
			{
				return ProcessStatus{ boost::none, DBG_EXCEPTION_NOT_HANDLED };
			}
			case IDebugEventsHandler::ExceptionType::Error:
			{
				return ProcessStatus{ boost::none, DBG_EXCEPTION_NOT_HANDLED };
			}
			case IDebugEventsHandler::ExceptionType::CppError:
			{
				if (continueAfterCppException_)
				{
					const auto& exceptionRecord = exception.ExceptionRecord;
					LOG_WARNING << "Continue after a C++ exception.";
					return ProcessStatus{ static_cast<int>(exceptionRecord.ExceptionCode), DBG_CONTINUE };
				}
				return ProcessStatus{ boost::none, DBG_EXCEPTION_NOT_HANDLED };
			}
		}
		THROW("Invalid exception Type.");
	}

	//-------------------------------------------------------------------------
	void Debugger::OnCreateProcess(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler)
	{		
		const auto& processInfo = debugEvent.u.CreateProcessInfo;
		Tools::ScopedAction scopedAction{ [&processInfo]{ CloseHandle(processInfo.hFile); } };

		LOG_DEBUG << "Create Process:" << debugEvent.dwProcessId;

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
		OnExitThread(debugEvent.dwThreadId);
		auto processId = debugEvent.dwProcessId;

		LOG_DEBUG << "Exit Process:" << processId;

		auto exitProcess = debugEvent.u.ExitProcess;
		debugEventsHandler.OnExitProcess(hProcess, hThread, exitProcess);

		if (processHandles_.erase(processId) != 1)
			THROW("Cannot find exited process.");

		return exitProcess.dwExitCode;
	}

	//-------------------------------------------------------------------------
	void Debugger::OnCreateThread(
		HANDLE hThread,
		DWORD dwThreadId)
	{
		LOG_DEBUG << "Create Thread:" << dwThreadId;

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

	//-------------------------------------------------------------------------
	size_t Debugger::GetRunningProcesses() const
	{
		return processHandles_.size();
	}

	//-------------------------------------------------------------------------
	size_t Debugger::GetRunningThreads() const
	{
		return threadHandles_.size();
	}
}