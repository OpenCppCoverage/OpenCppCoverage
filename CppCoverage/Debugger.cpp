#include "stdafx.h"
#include "Debugger.hpp"

#include "Process.hpp"
#include "CppCoverageException.hpp"
#include "IDebugEventsHandler.hpp"

namespace CppCoverage
{
	void Debugger::Debug(const StartInfo& startInfo, IDebugEventsHandler& debugEventsHandler)
	{
		Process process(startInfo);
		process.Start(DEBUG_ONLY_THIS_PROCESS | PROCESS_VM_READ); // $$ try to remove second and first is not general !!!);
		
		DEBUG_EVENT debugEvent;
		bool processHasExited = false;
		bool isFirstBreakPoint = true;
		
		while (!processHasExited)
		{
			if (!WaitForDebugEvent(&debugEvent, INFINITE))
				THROW_LAST_ERROR(L"Error WaitForDebugEvent:", GetLastError());
			
			switch (debugEvent.dwDebugEventCode)
			{
				case CREATE_PROCESS_DEBUG_EVENT:
				{
					const auto& processInfo = debugEvent.u.CreateProcessInfo;
					debugEventsHandler.OnCreateProcess(processInfo);

					CloseHandle(processInfo.hFile);
					break;
				}
				case EXIT_PROCESS_DEBUG_EVENT:
				{
					debugEventsHandler.OnExitProcess(debugEvent.u.ExitProcess);
					processHasExited = true; 
					break;
				} 
				case LOAD_DLL_DEBUG_EVENT:
				{
					const auto& loadDll = debugEvent.u.LoadDll;
					debugEventsHandler.OnLoadDll(loadDll); break;
					CloseHandle(loadDll.hFile); // $$ check for other evetns not listed here. Replace by handle
				}
				case EXCEPTION_DEBUG_EVENT: debugEventsHandler.OnException(debugEvent.u.Exception); break;
			}									

			if (!ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, DBG_CONTINUE))
				THROW_LAST_ERROR("Error in ContinueDebugEvent:", GetLastError());
		}
	}

}