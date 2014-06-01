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

#include "DebugInformationEventHandlerMock.hpp"

#include "CppCoverage/DebugInformation.hpp"
#include "Tools/Tool.hpp"
#include "TestTools.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		//---------------------------------------------------------------------
		void LoadModule(DebugInformationEventHandlerMock& mock, HANDLE hProcess, HANDLE hFile)
		{
			cov::DebugInformation debugInformation{ hProcess};
			
			debugInformation.LoadModule(L"", hFile, nullptr, mock);			
		}
	}

	//-------------------------------------------------------------------------
	TEST(DebugInformationTest, LoadModule)
	{						
		DebugInformationEventHandlerMock mock;

		EXPECT_CALL(mock, IsSourceFileSelected(testing::_))
			.Times(testing::AnyNumber())
			.WillRepeatedly(testing::Return(false));		
		EXPECT_CALL(mock, IsSourceFileSelected(
			TestCoverageConsole::GetMainCppPath().wstring()))
			.WillOnce(testing::Return(true));				
		EXPECT_CALL(mock, OnNewLine(testing::_, testing::_, testing::_))
			.Times(testing::AtLeast(1));

		TestTools::GetHandles(TestCoverageConsole::GetOutputBinaryPath(), [&](HANDLE hProcess, HANDLE hFile)
		{ 
			LoadModule(mock, hProcess, hFile);
		});				
	}
}