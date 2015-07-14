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

#include <Windows.h>
#include <boost/filesystem.hpp>

#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "OpenCppCoverage/OpenCppCoverage.hpp"
#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/ProgramOptions.hpp"

#include "Tools/ScopedAction.hpp"
#include "Tools/Tool.hpp"
#include "TestHelper/TemporaryPath.hpp"

#include "OpenCppCoverageTestTools.hpp"

namespace fs = boost::filesystem;
namespace cov = CppCoverage;

namespace OpenCppCoverageTest
{	
	namespace
	{
		//---------------------------------------------------------------------
		void CheckOutputDirectory(const TestHelper::TemporaryPath& tempFolder)
		{			
			ASSERT_FALSE(fs::is_empty(tempFolder.GetPath()));
		}

		//---------------------------------------------------------------------
		int RunCoverageForProgram(
			const fs::path& programToRun,
			const std::vector<std::wstring>& arguments)
		{
			TestHelper::TemporaryPath tempFolder;
			
			std::vector<std::pair<std::string, std::string>> coverageArguments;
			AddDefaultFilters(coverageArguments, programToRun);			
			coverageArguments.push_back(BuildExportTypeString(cov::ProgramOptions::ExportTypeHtmlValue, tempFolder.GetPath()));

			int exitCode = RunCoverageFor(coverageArguments, programToRun, arguments);
			
			CheckOutputDirectory(tempFolder);
			return exitCode;
		}
	}

	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, Basic)
	{		
		fs::path testCoverageConsole = TestCoverageConsole::GetOutputBinaryPath();
							
		ASSERT_EQ(0, RunCoverageForProgram(testCoverageConsole, {}));
	}	

	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, UnhandledException)
	{
		fs::path testCoverageConsole = TestCoverageConsole::GetOutputBinaryPath();

		ASSERT_NE(0, RunCoverageForProgram(testCoverageConsole, { TestCoverageConsole::TestThrowUnHandledSEHException }));
	}
	
	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, Breakpoint)
	{
		fs::path testCoverageConsole = TestCoverageConsole::GetOutputBinaryPath();

		ASSERT_EQ(EXCEPTION_BREAKPOINT, RunCoverageForProgram(testCoverageConsole, { TestCoverageConsole::TestBreakPoint }));
	}

	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, BreakpointChildProcess)
	{
		fs::path testCoverageConsole = TestCoverageConsole::GetOutputBinaryPath();

		ASSERT_EQ(0, RunCoverageForProgram(testCoverageConsole,
			{ TestCoverageConsole::TestChildProcess, TestCoverageConsole::TestBreakPoint }));
	}

	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, ExporterTest)
	{
		fs::path exporterTest{ OUT_DIR };
		
		exporterTest /= "ExporterTest.exe";
		ASSERT_EQ(0, RunCoverageForProgram(exporterTest, {}));
	}	
}