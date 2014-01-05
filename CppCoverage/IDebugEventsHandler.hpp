#ifndef CPPCOVERAGE_IDEBUGEVENTSHANDLER_HEADER_GARD
#define CPPCOVERAGE_IDEBUGEVENTSHANDLER_HEADER_GARD

#include <Windows.h>

#include "Export.hpp"

namespace CppCoverage
{
	class DLL IDebugEventsHandler
	{
	public:
		IDebugEventsHandler() = default;
		virtual ~IDebugEventsHandler() = default;

		virtual void OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO&);
		virtual void OnExitProcess(const EXIT_PROCESS_DEBUG_INFO&);
		virtual void OnLoadDll(const LOAD_DLL_DEBUG_INFO&);
		virtual void OnException(const EXCEPTION_DEBUG_INFO&);
		
	private:
		IDebugEventsHandler(const IDebugEventsHandler&) = delete;
		IDebugEventsHandler& operator=(const IDebugEventsHandler&) = delete;
	};

	inline void IDebugEventsHandler::OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO&) {}
	inline void IDebugEventsHandler::OnExitProcess(const EXIT_PROCESS_DEBUG_INFO&) {}
	inline void IDebugEventsHandler::OnLoadDll(const LOAD_DLL_DEBUG_INFO&) {}
	inline void IDebugEventsHandler::OnException(const EXCEPTION_DEBUG_INFO&) {}
}

#endif
