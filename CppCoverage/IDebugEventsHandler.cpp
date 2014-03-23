#include "stdafx.h"
#include "IDebugEventsHandler.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	void IDebugEventsHandler::OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO&) 
	{
	}

	//-------------------------------------------------------------------------
	void IDebugEventsHandler::OnExitProcess(HANDLE hProcess, HANDLE hThread, const EXIT_PROCESS_DEBUG_INFO&)
	{
	}

	//-------------------------------------------------------------------------
	void IDebugEventsHandler::OnLoadDll(HANDLE hProcess, HANDLE hThread, const LOAD_DLL_DEBUG_INFO&)
	{
	}

	//-------------------------------------------------------------------------
	DWORD IDebugEventsHandler::OnException(HANDLE hProcess, HANDLE hThread, const EXCEPTION_DEBUG_INFO&)
	{ 
		return DBG_EXCEPTION_NOT_HANDLED; 
	}
}
