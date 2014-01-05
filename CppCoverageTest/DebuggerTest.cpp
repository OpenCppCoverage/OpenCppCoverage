#include "stdafx.h"

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Debugger.hpp"

#include "DebugEventsMock.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	TEST(DebugerTest, Debug)
	{		
		cov::StartInfo startInfo{ Tools::GetConsoleForCppCoverageTest() };
		cov::Debugger debugger;
		DebugEventsHandlerMock debugEventsHandlerMock;

		EXPECT_CALL(debugEventsHandlerMock, OnCreateProcess(testing::_));
		EXPECT_CALL(debugEventsHandlerMock, OnExitProcess(testing::_));
		EXPECT_CALL(debugEventsHandlerMock, OnLoadDll(testing::_)).Times(testing::AnyNumber());;
		EXPECT_CALL(debugEventsHandlerMock, OnException(testing::_));

		debugger.Debug(startInfo, debugEventsHandlerMock);
	}
}