#include "stdafx.h"
#include "Debugger.hpp"

#include "tools/Log.hpp"

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

			auto continueStatus = DBG_CONTINUE;
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
						continueStatus = debugEventsHandler.OnException(hProcess, hThread, debugEvent.u.Exception);
						break;
					}
					case RIP_EVENT: 
					{
						OnRip(debugEvent.u.RipInfo);
						processHasExited = true;
						break;
					}
					default:
						LOG_DEBUG << "Debug event:" << debugEvent.dwDebugEventCode;
				}
			}
			if (!ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, continueStatus))
				THROW_LAST_ERROR("Error in ContinueDebugEvent:", GetLastError());
		}
	}

	//-------------------------------------------------------------------------
	void Debugger::OnCreateProcess(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler)
	{		
		const auto& processInfo = debugEvent.u.CreateProcessInfo;

		LOG_DEBUG << "Create new Process:" << debugEvent.dwProcessId;

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
		LOG_DEBUG << "Exit Process:" << hProcess;

		debugEventsHandler.OnExitProcess(hProcess, hThread, debugEvent.u.ExitProcess);

		if (processHandles_.erase(debugEvent.dwProcessId) != 1)
			THROW("Cannot find exited process.");		
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