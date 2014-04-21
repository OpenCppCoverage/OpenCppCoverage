#include "stdafx.h"

#include <boost/filesystem.hpp>

#include "CppCoverage/HandleInformation.hpp"

namespace cov = CppCoverage;
namespace bfs = boost::filesystem;

namespace CppCoverageTest
{	
	//-------------------------------------------------------------------------
	TEST(HandleInformationTest, ComputeFilename)
	{
		std::wstring consoleForCppCoverageTest = Tools::GetConsoleForCppCoverageTestPath();
		bfs::path path;

		Tools::GetHandles(consoleForCppCoverageTest, [&](HANDLE hProcess, HANDLE hFile) 
		{
			cov::HandleInformation handleInformation;

			path = handleInformation.ComputeFilename(hFile);
		});

		ASSERT_EQ(path, bfs::path(consoleForCppCoverageTest));
	}
}