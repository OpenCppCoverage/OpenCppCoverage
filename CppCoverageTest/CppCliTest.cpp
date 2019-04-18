// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2017 OpenCppCoverage
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

#include "CppCoverageTest/TestTools.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "TestCoverageSharedLib/TestCoverageSharedLib.hpp"

#include "TestHelper/Tools.hpp"

namespace fs = std::filesystem;

namespace CppCoverageTest
{
	//---------------------------------------------------------------------
	TEST(CppCliTest, ManagedUnManagedModule)
	{
		auto vsPath = TestHelper::GetVisualStudioPath();
		fs::path vsConsoleTestPath = vsPath / "Common7" / "IDE" /
		                             "CommonExtensions" / "Microsoft" /
		                             "TestWindow" / "vstest.console.exe";
		auto testCppCliPath = (fs::path{OUT_DIR} / "DefaultTest.dll").wstring();
		auto sharedLibModulePath = TestCoverageSharedLib::GetOutputBinaryPath();

		TestTools::CoverageArgs coverageArgs(
		    {testCppCliPath}, testCppCliPath, L"None");
		coverageArgs.modulePatternCollection_.push_back(
		    sharedLibModulePath.wstring());
		coverageArgs.programToRun_ = vsConsoleTestPath;

		auto coverage = TestTools::ComputeCoverageDataPatterns(coverageArgs);
		ASSERT_EQ(0, coverage.GetExitCode());
		const auto& modules = coverage.GetModules();
		ASSERT_EQ(1, modules.size());
		ASSERT_EQ(sharedLibModulePath, modules.at(0)->GetPath());
	}
}