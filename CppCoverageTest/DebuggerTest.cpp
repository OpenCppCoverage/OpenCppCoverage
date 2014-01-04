#include "stdafx.h"

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Debugger.hpp"
#include "CppCoverage/IDebugEvents.hpp"

#include "DebugEventsMock.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	TEST(DebugerTest, Debug)
	{		
		cov::StartInfo startInfo{ Tools::GetConsoleForCppCoverageTest() };
		cov::Debugger debugger;
		DebugEventsMock debugEventsMock;

		EXPECT_CALL(debugEventsMock, OnCreateProcess(testing::_));
		EXPECT_CALL(debugEventsMock, OnExitProcess(testing::_));
		EXPECT_CALL(debugEventsMock, OnLoadDll(testing::_)).Times(testing::AnyNumber());;
		EXPECT_CALL(debugEventsMock, OnException(testing::_));

		debugger.Debug(startInfo, debugEventsMock);
	}
}