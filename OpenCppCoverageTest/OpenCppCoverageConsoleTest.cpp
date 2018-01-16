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
#include <boost/algorithm/string/predicate.hpp>

#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "OpenCppCoverage/OpenCppCoverage.hpp"
#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/ProgramOptions.hpp"
#include "CppCoverage/FilterAssistant.hpp"

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
		    const std::vector<std::pair<std::string, std::string>>&
		        additionalCoverageArguments,
		    const fs::path& programToRun,
		    const std::vector<std::wstring>& arguments)
		{
			auto coverageArguments = additionalCoverageArguments;
			AddDefaultFilters(coverageArguments, programToRun);
			coverageArguments.emplace_back(cov::ProgramOptions::QuietOption,
			                               "");
			std::string ignoreOutput;
			return RunCoverageFor(
			    coverageArguments, programToRun, arguments, &ignoreOutput);
		}

		//---------------------------------------------------------------------
		int RunCoverageForProgram(const fs::path& programToRun,
		                          const std::vector<std::wstring>& arguments)
		{
			TestHelper::TemporaryPath tempFolder;

			int exitCode = RunCoverageForProgram(
			{ BuildExportTypeString(cov::ProgramOptions::ExportTypeHtmlValue,
									  tempFolder.GetPath()) },
			    programToRun,
			    arguments);

			CheckOutputDirectory(tempFolder);
			return exitCode;
		}

		//---------------------------------------------------------------------
		std::wstring
		GetOutput(const std::pair<std::string, std::string>& coverageArgument)
		{
			std::string output;
			auto exitCode =
			    RunCoverageFor({coverageArgument},
			                   TestCoverageConsole::GetOutputBinaryPath(),
			                   {},
			                   &output);
			if (exitCode != 0)
				throw std::runtime_error("Exit code invalid");
			return Tools::LocalToWString(output);
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

	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, ExitCode)
	{
		TestHelper::TemporaryPath tempFolder;

		const fs::path testCoverageConsole =
		    TestCoverageConsole::GetOutputBinaryPath();
		std::vector<std::pair<std::string, std::string>>
		    additionalCoverageArguments = {BuildExportTypeString(
		        cov::ProgramOptions::ExportTypeBinaryValue,
		        tempFolder.GetPath())};

		int exitCode = RunCoverageForProgram(
		    additionalCoverageArguments,
		    testCoverageConsole,
		    {TestCoverageConsole::TestThrowUnHandledCppException});
		ASSERT_NE(0, exitCode);
		ASSERT_TRUE(fs::exists(tempFolder));

		additionalCoverageArguments.emplace_back(
		    cov::ProgramOptions::InputCoverageValue, tempFolder->string());
		exitCode = RunCoverageForProgram(
		    additionalCoverageArguments,
		    testCoverageConsole,
		    {TestCoverageConsole::TestBasic});
		ASSERT_EQ(0, exitCode);
	}

	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, FilterAssistantNoModules)
	{
		ASSERT_TRUE(boost::algorithm::contains(
		    GetOutput({cov::ProgramOptions::SelectedModulesOption, "No match"}),
		    cov::FilterAssistant::NoModulesSelectedMsg));
	}

	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, FilterAssistantNoSourceFiles)
	{
		ASSERT_TRUE(boost::algorithm::contains(
		    GetOutput({cov::ProgramOptions::SelectedSourcesOption, "No match"}),
		    cov::FilterAssistant::NoSourceFilesSelectedMsg));
	}
}