// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Debugger.hpp"
#include "TestCoverageConsole/TestCoverageConsole.hpp"

#include "DebugEventsMock.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	//-----------------------------------------------------------------------------
	TEST(DebugerTest, Debug)
	{		
		cov::StartInfo startInfo{ TestCoverageConsole::GetOutputBinaryPath() };
		cov::Debugger debugger{ false, false, false, false };
		DebugEventsHandlerMock debugEventsHandlerMock;

		EXPECT_CALL(debugEventsHandlerMock, OnCreateProcess(testing::_));
		EXPECT_CALL(debugEventsHandlerMock, OnExitProcess(testing::_, testing::_, testing::_));
		EXPECT_CALL(debugEventsHandlerMock, OnLoadDll(testing::_, testing::_, testing::_)).Times(testing::AnyNumber());
		EXPECT_CALL(debugEventsHandlerMock, OnUnloadDll(testing::_, testing::_, testing::_)).Times(testing::AnyNumber());
		EXPECT_CALL(debugEventsHandlerMock, OnException(testing::_, testing::_, testing::_))
			.WillRepeatedly(testing::Return(cov::IDebugEventsHandler::ExceptionType::NotHandled));

		debugger.Debug(startInfo, debugEventsHandlerMock);
		ASSERT_EQ(0, debugger.GetRunningProcesses());
		ASSERT_EQ(0, debugger.GetRunningThreads());
	}	
}