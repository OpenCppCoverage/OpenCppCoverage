#ifndef CPPCOVERAGETEST_DEBUGEVENTSMOCK_HEADER_GARD
#define CPPCOVERAGETEST_DEBUGEVENTSMOCK_HEADER_GARD

#include "CppCoverage/IDebugEventsHandler.hpp"

namespace CppCoverageTest
{
	class DebugEventsHandlerMock : public CppCoverage::IDebugEventsHandler
	{
	public:
		DebugEventsHandlerMock() = default;

		MOCK_METHOD1(OnCreateProcess, void(const CREATE_PROCESS_DEBUG_INFO&));
		MOCK_METHOD3(OnExitProcess, void(HANDLE, HANDLE, const EXIT_PROCESS_DEBUG_INFO&));
		MOCK_METHOD3(OnLoadDll, void(HANDLE, HANDLE, const LOAD_DLL_DEBUG_INFO&));
		MOCK_METHOD3(OnException, void(HANDLE, HANDLE, const EXCEPTION_DEBUG_INFO&));

	private:
		DebugEventsHandlerMock(const DebugEventsHandlerMock&) = delete;
		DebugEventsHandlerMock& operator=(const DebugEventsHandlerMock&) = delete;
	};
}

#endif
