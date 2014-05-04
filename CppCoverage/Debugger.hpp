#pragma once

#include <unordered_map>
#include <Windows.h>
#include "CppCoverageExport.hpp"


namespace CppCoverage
{
	class StartInfo;
	class IDebugEventsHandler;

	class CPPCOVERAGE_DLL Debugger
	{
	public:
		Debugger() = default;

		int Debug(const StartInfo&, IDebugEventsHandler&);

	private:
		Debugger(const Debugger&) = delete;
		Debugger& operator=(const Debugger&) = delete;
	
		void OnCreateProcess(
			const DEBUG_EVENT& debugEvent,
			IDebugEventsHandler& debugEventsHandler);

		int OnExitProcess(
			const DEBUG_EVENT& debugEvent,
			HANDLE hProcess,
			HANDLE hThread,
			IDebugEventsHandler& debugEventsHandler);

		void OnCreateThread(
			HANDLE hThread,
			DWORD dwThreadId);

		void OnExitThread(DWORD dwProcessId);

		HANDLE GetProcessHandle(DWORD dwProcessId) const;
		HANDLE GetThreadHandle(DWORD dwThreadId) const;

		struct ProcessStatus;
		ProcessStatus HandleNotCreateEvent(
			const DEBUG_EVENT& debugEvent,
			IDebugEventsHandler& debugEventsHandler,
			HANDLE hProcess,
			HANDLE hThread,
			DWORD dwThreadId);

	private:
		std::unordered_map<DWORD, HANDLE> processHandles_;
		std::unordered_map<DWORD, HANDLE> threadHandles_;
	};
}


