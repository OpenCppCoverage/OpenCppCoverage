#pragma once

#include <Windows.h>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL IDebugEventsHandler
	{
	public:
		IDebugEventsHandler() = default;
		virtual ~IDebugEventsHandler() = default;

		virtual void OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO&);
		virtual void OnExitProcess(HANDLE hProcess, HANDLE hThread, const EXIT_PROCESS_DEBUG_INFO&);
		virtual void OnLoadDll(HANDLE hProcess, HANDLE hThread, const LOAD_DLL_DEBUG_INFO&);
		virtual DWORD OnException(HANDLE hProcess, HANDLE hThread, const EXCEPTION_DEBUG_INFO&);
		
	private:
		IDebugEventsHandler(const IDebugEventsHandler&) = delete;
		IDebugEventsHandler& operator=(const IDebugEventsHandler&) = delete;
	};	
}


