#include "stdafx.h"

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Debugger.hpp"

#include "DebugEventsMock.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	TEST(DebugerTest, Debug)
	{		
		cov::StartInfo startInfo{ Tools::GetConsoleForCppCoverageTestPath() };
		cov::Debugger debugger;
		DebugEventsHandlerMock debugEventsHandlerMock;

		EXPECT_CALL(debugEventsHandlerMock, OnCreateProcess(testing::_));
		EXPECT_CALL(debugEventsHandlerMock, OnExitProcess(testing::_, testing::_, testing::_));
		EXPECT_CALL(debugEventsHandlerMock, OnLoadDll(testing::_, testing::_, testing::_)).Times(testing::AnyNumber());;
		EXPECT_CALL(debugEventsHandlerMock, OnException(testing::_, testing::_, testing::_))
			.WillRepeatedly(testing::Return(DBG_CONTINUE));

		debugger.Debug(startInfo, debugEventsHandlerMock);
	}
}