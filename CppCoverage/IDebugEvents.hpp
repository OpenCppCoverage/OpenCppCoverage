#ifndef CPPCOVERAGE_IDEBUGEVENTS_HEADER_GARD
#define CPPCOVERAGE_IDEBUGEVENTS_HEADER_GARD

#include <Windows.h>

#include "Export.hpp"

namespace CppCoverage
{
	class DLL IDebugEvents // $$ rename to IDebugEvents handler
	{
	public:
		IDebugEvents() = default;
		virtual ~IDebugEvents() = default;

		virtual void OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO&);
		virtual void OnExitProcess(const EXIT_PROCESS_DEBUG_INFO&);
		virtual void OnLoadDll(const LOAD_DLL_DEBUG_INFO&);
		virtual void OnException(const EXCEPTION_DEBUG_INFO&);
		
	private:
		IDebugEvents(const IDebugEvents&) = delete;
		IDebugEvents& operator=(const IDebugEvents&) = delete;
	};

	inline void IDebugEvents::OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO&) {}
	inline void IDebugEvents::OnExitProcess(const EXIT_PROCESS_DEBUG_INFO&) {}
	inline void IDebugEvents::OnLoadDll(const LOAD_DLL_DEBUG_INFO&) {}
	inline void IDebugEvents::OnException(const EXCEPTION_DEBUG_INFO&) {}
}

#endif
