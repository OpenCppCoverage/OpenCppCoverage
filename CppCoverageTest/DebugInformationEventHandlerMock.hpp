#ifndef CPPCOVERAGETEST_DEBUGINFORMATIONEVENTHANDLERMOCK_HEADER_GARD
#define CPPCOVERAGETEST_DEBUGINFORMATIONEVENTHANDLERMOCK_HEADER_GARD

#include "CppCoverage/IDebugInformationEventHandler.hpp"

namespace CppCoverageTest
{
	class DebugInformationEventHandlerMock : public CppCoverage::IDebugInformationEventHandler
	{
	public:
		DebugInformationEventHandlerMock() = default;
		
		MOCK_CONST_METHOD1(IsSourceFileSelected, bool(const std::string&));
		MOCK_METHOD3(OnNewLine, void(const std::string& fileName, int lineNumber, DWORD64 address));

	private:
		DebugInformationEventHandlerMock(const DebugInformationEventHandlerMock&) = delete;
		DebugInformationEventHandlerMock& operator=(const DebugInformationEventHandlerMock&) = delete;
	};
}

#endif
