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

#include <boost/algorithm/string/predicate.hpp>

#include "DebugInformationEventHandlerMock.hpp"
#include "CoverageFilterManagerMock.hpp"

#include "CppCoverage/DebugInformation.hpp"
#include "CppCoverage/Address.hpp"

#include "Tools/Tool.hpp"
#include "TestTools.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		//---------------------------------------------------------------------
		void LoadModule(
			CoverageFilterManagerMock& filterManagerMock,
			DebugInformationEventHandlerMock& eventHandlerMock,
			HANDLE hProcess,
			HANDLE hFile)
		{
			cov::DebugInformation debugInformation{ hProcess};
			
			debugInformation.LoadModule(L"", hFile, nullptr, filterManagerMock, eventHandlerMock);
		}
	}

	//-------------------------------------------------------------------------
	TEST(DebugInformationTest, LoadModule)
	{						
		DebugInformationEventHandlerMock eventHandlerMock;
		CoverageFilterManagerMock filterManagerMock;
		auto mainCppPath = TestCoverageConsole::GetMainCppFilename().wstring();

		std::vector<int> selectedLines;
		int lineSelectedCallCount = 0;

		EXPECT_CALL(filterManagerMock, IsLineSelected(testing::_, testing::_))
			.WillRepeatedly(testing::Invoke(
				[&](const std::wstring& path, int lineNumber)
		{
			++lineSelectedCallCount;

			if (lineNumber % 2 == 0)
				return false;
			selectedLines.push_back(lineNumber);
			return true;
		}));

		EXPECT_CALL(filterManagerMock, IsSourceFileSelected(testing::_))
			.WillRepeatedly(testing::Invoke(
			[=](const std::wstring& path){ return boost::algorithm::icontains(path, mainCppPath); }));

		std::vector<int> newLines;
		EXPECT_CALL(eventHandlerMock, OnNewLine(testing::_, testing::_, testing::_))
			.WillRepeatedly(testing::Invoke(
				[&](const std::wstring&, int lineNumber, const cov::Address&)
		{
			newLines.push_back(lineNumber);
		}));

		TestTools::GetHandles(TestCoverageConsole::GetOutputBinaryPath(), [&](HANDLE hProcess, HANDLE hFile)
		{ 
			LoadModule(filterManagerMock, eventHandlerMock, hProcess, hFile);
		});

		ASSERT_NE(0, selectedLines.size());
		ASSERT_NE(lineSelectedCallCount, selectedLines.size());
		ASSERT_EQ(selectedLines, newLines);
	}
}