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

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "TestHelper/TemporaryPath.hpp"
#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/ProgramOptions.hpp"
#include "OpenCppCoverage/OpenCppCoverage.hpp"
#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "TestCoverageSharedLib/TestCoverageSharedLib.hpp"

#include "OpenCppCoverageTestTools.hpp"

namespace fs = boost::filesystem;
namespace cov = CppCoverage;

namespace OpenCppCoverageTest
{
	namespace
	{
		auto testCoverageConsole = TestCoverageConsole::GetOutputBinaryPath();
		auto testCoverageSharedLib = TestCoverageSharedLib::GetOutputBinaryPath();
		
		auto testCoverageSharedLibMain = TestCoverageSharedLib::GetMainCppPath();

		//---------------------------------------------------------------------
		bool Find(const std::wstring& filename, const fs::path& outputDirectory)
		{
			for (fs::recursive_directory_iterator it(outputDirectory);
				it != fs::recursive_directory_iterator(); ++it)
			{
				const auto& path = it->path();

				if (path.filename().wstring().find(filename) != std::string::npos)
					return true;
			}

			return false;
		}
		
		//---------------------------------------------------------------------
		class CommandLineOptionsTest : public ::testing::Test
		{
		public:
			//-----------------------------------------------------------------
			void RunCoverageOnProgram(
				std::vector<std::pair<std::string, std::string>> coverageArguments,
				bool useSourceInSolutionDir = true)
			{
				if (useSourceInSolutionDir)
					coverageArguments.push_back({ cov::ProgramOptions::SelectedSourcesOption, GetSolutionFolderName()});
				coverageArguments.push_back({ cov::ProgramOptions::ExcludedSourcesOption, "packages" });
				AddDefaultHtmlExport(coverageArguments, GetTempPath());
				coverageArguments.emplace_back(cov::ProgramOptions::QuietOption, "");
				int exitCode = RunCoverageFor(coverageArguments, testCoverageConsole, {});

				ASSERT_EQ(0, exitCode);
			}

			//-----------------------------------------------------------------
			void CheckFilenameExistsInOutput(const fs::path& path, bool expectedValue)
			{
				auto filename = path.filename().replace_extension("");
				CheckFilenameWithExtensionExistsInOutput(filename, expectedValue);
			}

			//-----------------------------------------------------------------
			void CheckFilenameWithExtensionExistsInOutput(const fs::path& path, bool expectedValue)
			{
				auto filename = path.filename();
				ASSERT_EQ(expectedValue, Find(filename.wstring(), GetTempPath()));
			}

			//-----------------------------------------------------------------
			const fs::path& GetTempPath() const
			{
				return tempFolder_.GetPath();
			}

		private:
			TestHelper::TemporaryPath tempFolder_;
		};
	}
	
	//-------------------------------------------------------------------------
	TEST_F(CommandLineOptionsTest, SelectedModulesOption)
	{		
		RunCoverageOnProgram({ { cov::ProgramOptions::SelectedModulesOption, testCoverageConsole.string() } });
		CheckFilenameExistsInOutput(testCoverageConsole, true);
		CheckFilenameExistsInOutput(testCoverageSharedLib, false);
	}

	//-------------------------------------------------------------------------
	TEST_F(CommandLineOptionsTest, ExcludedModulesOption)
	{
		RunCoverageOnProgram({ { cov::ProgramOptions::ExcludedModulesOption, testCoverageConsole.string() } });
		CheckFilenameExistsInOutput(testCoverageConsole, false);
		CheckFilenameExistsInOutput(testCoverageSharedLib, true);
	}

	//-------------------------------------------------------------------------
	TEST_F(CommandLineOptionsTest, SelectedSourcesOption)
	{
		auto testCoverageConsoleMain = TestCoverageConsole::GetMainCppFilename().string();

		boost::algorithm::to_lower(testCoverageConsoleMain);
		RunCoverageOnProgram({ { cov::ProgramOptions::SelectedSourcesOption, testCoverageConsoleMain } }, false);
		CheckFilenameWithExtensionExistsInOutput(testCoverageConsoleMain, true);
		CheckFilenameWithExtensionExistsInOutput(testCoverageSharedLibMain, false);
	}

	//-------------------------------------------------------------------------
	TEST_F(CommandLineOptionsTest, ExcludedSourcesOption)
	{
		auto testCoverageConsoleMain = TestCoverageConsole::GetMainCppFilename().string();

		boost::algorithm::to_lower(testCoverageConsoleMain);
		RunCoverageOnProgram({ { cov::ProgramOptions::ExcludedSourcesOption, testCoverageConsoleMain } });
		CheckFilenameWithExtensionExistsInOutput(testCoverageConsoleMain, false);
		CheckFilenameWithExtensionExistsInOutput(testCoverageSharedLibMain, true);
	}

	//-------------------------------------------------------------------------
	TEST_F(CommandLineOptionsTest, OutputDirectoryOption)
	{
		RunCoverageOnProgram({});
		CheckFilenameExistsInOutput(testCoverageConsole, true);
		CheckFilenameExistsInOutput(testCoverageSharedLib, true);
	}	
}
