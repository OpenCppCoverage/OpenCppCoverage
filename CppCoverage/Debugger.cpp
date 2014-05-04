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
		ProcessStatus(
			boost::optional<int> exitCode = boost::optional<int>(),
			DWORD continueStatus = DBG_CONTINUE)
			: exitCode_(exitCode)
			, continueStatus_(continueStatus)
		{
		}

		boost::optional<int> exitCode_;
		DWORD continueStatus_;
	};

	//-------------------------------------------------------------------------
	int Debugger::Debug(const StartInfo& startInfo, IDebugEventsHandler& debugEventsHandler)
	{
		Process process(startInfo);
		process.Start(DEBUG_ONLY_THIS_PROCESS);
		
		DEBUG_EVENT debugEvent;
		ProcessStatus processStatus;
								
		while (!processStatus.exitCode_)
		{
			if (!WaitForDebugEvent(&debugEvent, INFINITE))
				THROW_LAST_ERROR(L"Error WaitForDebugEvent:", GetLastError());
			
			auto dwProcessId = debugEvent.dwProcessId;
			auto dwThreadId = debugEvent.dwThreadId;						

			processStatus.continueStatus_ = DBG_CONTINUE;
			switch (debugEvent.dwDebugEventCode)
			{
				case CREATE_PROCESS_DEBUG_EVENT: OnCreateProcess(debugEvent, debugEventsHandler); break;
				case CREATE_THREAD_DEBUG_EVENT: OnCreateThread(debugEvent.u.CreateThread.hThread, dwThreadId); break;
				default:
				{
					auto hProcess = GetProcessHandle(dwProcessId);
					auto hThread = GetThreadHandle(dwThreadId);
					processStatus = HandleNotCreateEvent(debugEvent, debugEventsHandler, hProcess, hThread, dwThreadId);
				}
			}
			if (!ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, processStatus.continueStatus_))
				THROW_LAST_ERROR("Error in ContinueDebugEvent:", GetLastError());
		}

		return *processStatus.exitCode_;
	}
	
	//-------------------------------------------------------------------------
	Debugger::ProcessStatus
		Debugger::HandleNotCreateEvent(
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
				return ProcessStatus{exitCode};
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
				return ProcessStatus{ boost::optional<int>(), continueStatus };
			}
			case RIP_EVENT: OnRip(debugEvent.u.RipInfo); return ProcessStatus{ false };
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