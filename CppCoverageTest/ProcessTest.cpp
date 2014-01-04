#include "stdafx.h"

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Process.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	TEST(Process, Start)
	{
		cov::StartInfo startInfo{ Tools::GetConsoleForCppCoverageTest() };
		cov::Process process{ startInfo };
				
		ASSERT_NO_THROW(process.Start(0));
	}
}