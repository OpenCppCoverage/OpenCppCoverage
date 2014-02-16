#ifndef CPPCOVERAGE_DEBUGGER_HEADER_GARD
#define CPPCOVERAGE_DEBUGGER_HEADER_GARD

#include <unordered_map>

#include "Export.hpp"


namespace CppCoverage
{
	class StartInfo;
	class IDebugEventsHandler;

	class CPPCOVERAGE_DLL Debugger
	{
	public:
		Debugger() = default;

		void Debug(const StartInfo&, IDebugEventsHandler&);

	private:
		Debugger(const Debugger&) = delete;
		Debugger& operator=(const Debugger&) = delete;
	
		void OnCreateProcess(
			const DEBUG_EVENT& debugEvent,
			IDebugEventsHandler& debugEventsHandler);

		void OnExitProcess(
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

	private:
		std::unordered_map<DWORD, HANDLE> processHandles_;
		std::unordered_map<DWORD, HANDLE> threadHandles_;
	};
}

#endif
