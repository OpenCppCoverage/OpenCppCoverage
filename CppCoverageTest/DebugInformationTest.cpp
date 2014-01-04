#include "stdafx.h"

#include "DebugInformationEventHandlerMock.hpp"

#include "CppCoverage/DebugInformation.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		void LoadModule(DebugInformationEventHandlerMock& mock, HANDLE hProcess, HANDLE hFile)
		{
			cov::DebugInformation debugInformation{hProcess};
			
			debugInformation.LoadModule(hFile, mock);			
		}
	}

	TEST(DebugInformationTest, LoadModule)
	{				
		auto executableWString = Tools::GetConsoleForCppCoverageTest();
		std::string executable{executableWString.begin(), executableWString.end()};

		DebugInformationEventHandlerMock mock;

		EXPECT_CALL(mock, IsSourceFileSelected(testing::_))
			.Times(testing::AnyNumber())
			.WillRepeatedly(testing::Return(false));		
		EXPECT_CALL(mock, IsSourceFileSelected(testing::EndsWith("consoleforcppcoveragetest.cpp")))
			.WillOnce(testing::Return(true));				
		EXPECT_CALL(mock, OnNewLine(testing::_, testing::_, testing::_))
			.Times(testing::AtLeast(1));

		Tools::GetHandles(Tools::GetConsoleForCppCoverageTest(), [&](HANDLE hProcess, HANDLE hFile)
		{ 
			LoadModule(mock, hProcess, hFile);
		});				
	}
}