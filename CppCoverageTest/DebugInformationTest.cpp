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