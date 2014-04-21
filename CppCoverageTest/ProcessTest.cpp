#include "stdafx.h"

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Process.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	//-------------------------------------------------------------------------
	TEST(Process, Start)
	{
		cov::StartInfo startInfo{ Tools::GetConsoleForCppCoverageTestPath() };
		cov::Process process{ startInfo };
				
		ASSERT_NO_THROW(process.Start(0));
	}

	//-------------------------------------------------------------------------
	TEST(Process, StartWithArgument)
	{
		cov::StartInfo startInfo{ Tools::GetConsoleForCppCoverageTestPath() };
		startInfo.AddArguments(L"42");
		
		cov::Process process{ startInfo };
		ASSERT_NO_THROW(process.Start(0));
	}
}