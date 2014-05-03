#include "stdafx.h"

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Process.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	//-------------------------------------------------------------------------
	TEST(Process, Start)
	{
		cov::StartInfo startInfo{ TestCoverageConsole::GetOutputBinaryPath() };
		cov::Process process{ startInfo };
				
		ASSERT_NO_THROW(process.Start(0));
	}

	//-------------------------------------------------------------------------
	TEST(Process, StartWithArgument)
	{
		cov::StartInfo startInfo{ TestCoverageConsole::GetOutputBinaryPath() };
		startInfo.AddArguments(TestCoverageConsole::TestThrowHandledException);
		
		cov::Process process{ startInfo };
		ASSERT_NO_THROW(process.Start(0));
	}
}