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

#include <filesystem>

#include "CppCoverage/HandleInformation.hpp"
#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "Tools/Tool.hpp"

#include "TestTools.hpp"

namespace cov = CppCoverage;
namespace fs = std::filesystem;

namespace CppCoverageTest
{	
	//-------------------------------------------------------------------------
	TEST(HandleInformationTest, ComputeFilename)
	{
		auto outputBinaryPath = TestCoverageConsole::GetOutputBinaryPath();
		fs::path path;

		TestTools::GetHandles(outputBinaryPath, [&](HANDLE hProcess, HANDLE hFile)
		{
			cov::HandleInformation handleInformation;

			path = handleInformation.ComputeFilename(hFile);
		});

		ASSERT_EQ(outputBinaryPath, path);
	}
}