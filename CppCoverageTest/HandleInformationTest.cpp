#include "stdafx.h"

#include <boost/filesystem.hpp>

#include "CppCoverage/HandleInformation.hpp"
#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "Tools/Tool.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace CppCoverageTest
{	
	//-------------------------------------------------------------------------
	TEST(HandleInformationTest, ComputeFilename)
	{
		auto outputBinaryPath = TestCoverageConsole::GetOutputBinaryPath();
		fs::path path;

		Tools::GetHandles(outputBinaryPath, [&](HANDLE hProcess, HANDLE hFile)
		{
			cov::HandleInformation handleInformation;

			path = handleInformation.ComputeFilename(hFile);
		});

		ASSERT_EQ(outputBinaryPath, path);
	}
}