#include "stdafx.h"
#include "Debugger.hpp"

#include "Process.hpp"
#include "CppCoverageException.hpp"
#include "IDebugEventsHandler.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	void Debugger::Debug(const StartInfo& startInfo, IDebugEventsHandler& debugEventsHandler)
	{
		Process process(startInfo);
		process.Start(DEBUG_ONLY_THIS_PROCESS); // $$ manage child process too
		
		DEBUG_EVENT debugEvent;
		bool processHasExited = false;
				
		while (!processHasExited)
		{
			if (!WaitForDebugEvent(&debugEvent, INFINITE))
				THROW_LAST_ERROR(L"Error WaitForDebugEvent:", GetLastError());
			
			auto dwProcessId = debugEvent.dwProcessId;
			auto dwThreadId = debugEvent.dwThreadId;

			if (debugEvent.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT)
				OnCreateProcess(debugEvent, debugEventsHandler);
			else if (debugEvent.dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT)
				OnCreateThread(debugEvent.u.CreateThread.hThread, dwThreadId);
			else
			{
				auto hProcess = GetProcessHandle(dwProcessId);
				auto hThread = GetThreadHandle(dwThreadId);

				switch (debugEvent.dwDebugEventCode)
				{				
					case EXIT_PROCESS_DEBUG_EVENT:
					{
						OnExitProcess(debugEvent, hProcess, hThread, debugEventsHandler);
						processHasExited = true;
						break;
					}
					case EXIT_THREAD_DEBUG_EVENT: OnExitThread(dwThreadId); break;									
					case LOAD_DLL_DEBUG_EVENT:
					{
						const auto& loadDll = debugEvent.u.LoadDll;
						debugEventsHandler.OnLoadDll(hProcess, hThread, loadDll);
						CloseHandle(loadDll.hFile); // $$ check for other evetns not listed here. Replace by handle  $$ split this code
						break;
					}
					case EXCEPTION_DEBUG_EVENT:
					{
						debugEventsHandler.OnException(hProcess, hThread, debugEvent.u.Exception);
						break;
					}
				}
			}
			if (!ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, DBG_CONTINUE))
				THROW_LAST_ERROR("Error in ContinueDebugEvent:", GetLastError());
		}
	}

	//-------------------------------------------------------------------------
	void Debugger::OnCreateProcess(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler)
	{		
		const auto& processInfo = debugEvent.u.CreateProcessInfo;

		if (!processHandles_.emplace(debugEvent.dwProcessId, processInfo.hProcess).second)
			THROW("Process id already exist");
		
		debugEventsHandler.OnCreateProcess(processInfo);

		OnCreateThread(processInfo.hThread, debugEvent.dwThreadId);

		CloseHandle(processInfo.hFile);
	}

	//-------------------------------------------------------------------------
	void Debugger::OnExitProcess(
		const DEBUG_EVENT& debugEvent,
		HANDLE hProcess,
		HANDLE hThread,
		IDebugEventsHandler& debugEventsHandler)
	{
		debugEventsHandler.OnExitProcess(hProcess, hThread, debugEvent.u.ExitProcess);

		if (processHandles_.erase(debugEvent.dwProcessId) != 1)
			THROW("Cannot find exited process.");
	}

	//-------------------------------------------------------------------------
	void Debugger::OnCreateThread(
		HANDLE hThread,
		DWORD dwThreadId)
	{
		if (!threadHandles_.emplace(dwThreadId, hThread).second)
			THROW("Thread id already exist");
	}
	
	//-------------------------------------------------------------------------
	void Debugger::OnExitThread(DWORD dwProcessId)
	{				
		if (threadHandles_.erase(dwProcessId) != 1)
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