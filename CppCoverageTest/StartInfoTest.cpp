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

#include "CppCoverage/StartInfo.hpp"
#include "Tools/ExceptionBase.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

namespace cov = CppCoverage;
namespace fs = std::filesystem;

namespace CppCoverageTest
{
	namespace
	{
		const std::wstring validFilename = TestCoverageConsole::GetOutputBinaryPath().wstring(); 
	}

	//-------------------------------------------------------------------------
	TEST(StartInfoTest, Constructor)
	{
		ASSERT_NO_THROW(cov::StartInfo s{ validFilename };);
	}

	//-------------------------------------------------------------------------
	TEST(StartInfoTest, SetWorkingDirectoryNotExists)
	{
		cov::StartInfo s(validFilename);
		fs::path folder{ L"" };
		
		ASSERT_THROW(s.SetWorkingDirectory(folder), std::runtime_error);
	}

	//-------------------------------------------------------------------------
	TEST(StartInfoTest, SetWorkingDirectoryExists)
	{
		cov::StartInfo s(validFilename);
		fs::path folder{ L"." };

		ASSERT_NO_THROW(s.SetWorkingDirectory(folder));
	}

}