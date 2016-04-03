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
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/log/trivial.hpp>

#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/CoverageSettings.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"
#include "CppCoverage/Patterns.hpp"
#include "CppCoverage/ExceptionHandler.hpp"
#include "CppCoverage/CoverageDataMerger.hpp"
#include "CppCoverage/UnifiedDiffSettings.hpp"

#include "Tools/Log.hpp"
#include "Tools/Tool.hpp"

#include "TestHelper/CoverageDataComparer.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "TestCoverageConsole/TestBasic.hpp"
#include "TestCoverageConsole/TestThread.hpp"
#include "TestCoverageConsole/SpecialLineInfo.hpp"
#include "TestCoverageSharedLib/TestCoverageSharedLib.hpp"

#include "TestTools.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;
namespace logging = boost::log;

namespace CppCoverageTest
{
	//-------------------------------------------------------------------------
	class CodeCoverageRunnerTest : public ::testing::Test
	{
	public:
				
		//---------------------------------------------------------------------
		void SetUp() override
		{
			error_ = boost::make_shared<std::ostringstream>();

			Tools::EnableLogger(true);
			Tools::SetLoggerMinSeverity(logging::trivial::error);
			Tools::InitLoggerOstream(error_);
		}

		//---------------------------------------------------------------------
		void TearDown() override
		{
			Tools::EnableLogger(false);
		}

		//---------------------------------------------------------------------
		void TestLine(const cov::FileCoverage& file, unsigned int lineNumber, bool hasBeenExecuted)
		{
			const auto* line = file[lineNumber];

			ASSERT_NE(nullptr, line);
			ASSERT_EQ(hasBeenExecuted, line->HasBeenExecuted());
		}

		//---------------------------------------------------------------------
		cov::CoverageData ComputeCoverageDataPatterns(
			const std::vector<std::wstring>& arguments,
			const std::vector<std::wstring>& modulePatternCollection,
			const std::vector<std::wstring>& sourcePatternCollection,
			const std::vector<cov::UnifiedDiffSettings>& unifiedDiffSettingsCollection = {},
			bool coverChildren = true)
		{
			cov::CodeCoverageRunner codeCoverageRunner;
			cov::Patterns modulePatterns{false};
			cov::Patterns sourcePatterns{false};
			
			cov::CoverageSettings coverageSettings{modulePatterns, sourcePatterns};

			for (auto modulePattern : modulePatternCollection)
			{
				boost::to_lower(modulePattern);
				modulePatterns.AddSelectedPatterns(modulePattern);
			}

			for (auto sourcePattern : sourcePatternCollection)
			{
				boost::to_lower(sourcePattern);
				sourcePatterns.AddSelectedPatterns(sourcePattern);
			}
			
			cov::StartInfo startInfo{ TestCoverageConsole::GetOutputBinaryPath().wstring() };
			for (const auto& argument: arguments)
				startInfo.AddArgument(argument);

			auto coverageData = codeCoverageRunner.RunCoverage(
				startInfo, coverageSettings, unifiedDiffSettingsCollection, coverChildren, 0);
			if (codeCoverageRunner.GetDebugInformationCount() != 0)
				throw std::runtime_error("Invalid number of DebugInformation.");

			return coverageData;
		}

		//---------------------------------------------------------------------
		cov::CoverageData ComputeCoverageData(
			const std::vector<std::wstring>& arguments,
			const std::wstring& modulePattern,
			const std::wstring& sourcePattern,
			const std::vector<cov::UnifiedDiffSettings>& unifiedDiffSettingsCollection = {},
			bool coverChildren = true)
		{
			return ComputeCoverageDataPatterns(
				arguments, { modulePattern }, { sourcePattern }, unifiedDiffSettingsCollection, coverChildren);
		}

		//---------------------------------------------------------------------
		cov::CoverageData ComputeCoverageDataInBothMode(
			const std::wstring& programArg,
			std::wstring modulePattern,
			std::wstring sourcePattern)
		{
			std::vector<std::wstring> arguments = { programArg };

			auto coverageData = ComputeCoverageData(arguments, modulePattern, sourcePattern);

			// Run child child process.
			arguments.insert(arguments.begin(), TestCoverageConsole::TestChildProcess);
			auto coverageDataChildProcess = ComputeCoverageData(arguments, modulePattern, sourcePattern);

			if (!TestHelper::CoverageDataComparer().IsFirstCollectionContainsSecond(
					coverageDataChildProcess.GetModules(), coverageData.GetModules()))
			{
				throw std::runtime_error("Invalid coverage");
			}
			return coverageData;
		}

		//---------------------------------------------------------------------
		cov::CoverageData RunCoverageWithException(const std::wstring& programArg)
		{
			return ComputeCoverageDataInBothMode(
				programArg,
				TestCoverageConsole::GetOutputBinaryPath().wstring(),
				TestCoverageConsole::GetMainCppPath().wstring());
		}

		//---------------------------------------------------------------------
		cov::CoverageData ComputeCoverageData(const std::wstring& programArg, const std::wstring& fileArgument)
		{
			return ComputeCoverageDataInBothMode(
				programArg,
				TestCoverageConsole::GetOutputBinaryPath().filename().wstring(),
				fileArgument);
		}

		//---------------------------------------------------------------------
		cov::FileCoverage& GetFirstFileCoverage(const cov::CoverageData& coverageData)
		{
			const auto& modules = coverageData.GetModules();
			const auto& module = *modules.at(0);
			const auto& files = module.GetFiles();

			return *files.at(0);
		}

		//---------------------------------------------------------------------
		std::wstring GetError() const
		{
			return Tools::ToWString(error_->str());
		}		

	private:
		boost::shared_ptr<std::ostringstream> error_;		
	};

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, RunCoverage)
	{		
		const auto testBasicFilename = TestCoverageConsole::GetTestBasicFilename().wstring();
		
		cov::CoverageData coverageData = ComputeCoverageData(TestCoverageConsole::TestBasic, testBasicFilename);
		auto& file = GetFirstFileCoverage(coverageData);

		int line = TestCoverageConsole::GetTestBasicLine() + 1;
		TestLine(file, line++, true);
		TestLine(file, line++, true);
		ASSERT_TRUE(file[line++] == nullptr);
		TestLine(file, line++, false);
		ASSERT_EQ(0, coverageData.GetExitCode());
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, RunCoverageDll)
	{			
		auto mainSharedLibFile = TestCoverageSharedLib::GetMainCppPath().wstring();

		cov::CoverageData coverageData = ComputeCoverageDataInBothMode(TestCoverageConsole::TestSharedLib,
			TestCoverageSharedLib::GetOutputBinaryPath().wstring(), mainSharedLibFile);
		auto& file = GetFirstFileCoverage(coverageData);

		int line = 31;
		TestLine(file, line++, true);
		TestLine(file, line++, true);
		TestLine(file, line++, true);
		ASSERT_EQ(nullptr, file[line++]);
		TestLine(file, line++, false);
	}
	
	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, RunThread)
	{
		const auto filter = TestCoverageConsole::GetTestThreadFilename().wstring();

		cov::CoverageData coverageData = ComputeCoverageData(TestCoverageConsole::TestThread, filter);
		auto& file = GetFirstFileCoverage(coverageData);

		int line = 28;

		TestLine(file, line++, true);
		ASSERT_EQ(nullptr, file[line++]);
		TestLine(file, line++, true);
		TestLine(file, line++, true);
		TestLine(file, line++, true);
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, HandledException)
	{
		cov::CoverageData coverageData = RunCoverageWithException(TestCoverageConsole::TestThrowHandledException);
		ASSERT_EQ(std::string::npos, 
			GetError().find(cov::ExceptionHandler::UnhandledExceptionErrorMessage));
		ASSERT_EQ(0, coverageData.GetExitCode());
	}
	
	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, UnHandledSEHException)
	{
		cov::CoverageData coverageData = RunCoverageWithException(TestCoverageConsole::TestThrowUnHandledSEHException);
		ASSERT_NE(std::string::npos, 
			GetError().find(cov::ExceptionHandler::UnhandledExceptionErrorMessage));
		ASSERT_NE(0, coverageData.GetExitCode());
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, UnHandledCppException)
	{
		auto coverageData = RunCoverageWithException(TestCoverageConsole::TestThrowUnHandledCppException);
		ASSERT_NE(std::string::npos,
			GetError().find(cov::ExceptionHandler::UnhandledExceptionErrorMessage));
		ASSERT_EQ(cov::ExceptionHandler::CppExceptionErrorCode, coverageData.GetExitCode());
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, UnHandledCppExceptionContinue)
	{
		auto coverageData = RunCoverageWithException(TestCoverageConsole::TestThrowUnHandledCppException);
		auto& file = GetFirstFileCoverage(coverageData);
		auto returnLine = TestCoverageConsole::GetTestCoverageConsoleCppMainLine() + 31;
		TestLine(file, returnLine, true);
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, ChildProcess)
	{
		std::vector<std::wstring> arguments = {
			TestCoverageConsole::TestChildProcess,
			TestCoverageConsole::TestThrowHandledException };
		const auto modulePattern = TestCoverageConsole::GetOutputBinaryPath().wstring();
		const auto sourcePattern = TestCoverageConsole::GetMainCppFilename().wstring();

		auto rootAndChildProcess = ComputeCoverageData(arguments, modulePattern, sourcePattern, {}, true);
		auto rootProcessOnly = ComputeCoverageData(arguments, modulePattern, sourcePattern, {}, false);

		const auto& rootOnlyModules = rootProcessOnly.GetModules();
		const auto& rootAndChildModules = rootAndChildProcess.GetModules();

		ASSERT_TRUE(TestHelper::CoverageDataComparer().IsFirstCollectionContainsSecond(
			rootAndChildModules, rootOnlyModules));
		ASSERT_FALSE(TestHelper::CoverageDataComparer().IsFirstCollectionContainsSecond(
			rootOnlyModules, rootAndChildModules));
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, SeveralChildProcess)
	{
		auto coverageData = ComputeCoverageData(
			{	TestCoverageConsole::TestChildProcess,
				TestCoverageConsole::TestThrowUnHandledCppException,
				TestCoverageConsole::TestThrowUnHandledSEHException },
			TestCoverageConsole::GetOutputBinaryPath().wstring(),
			TestCoverageConsole::GetMainCppFilename().wstring());

		std::vector<cov::CoverageData> coverageDataCollection;
		coverageDataCollection.push_back(std::move(coverageData));
		auto mergedCoverageData = cov::CoverageDataMerger().Merge(coverageDataCollection);

		auto& file = GetFirstFileCoverage(mergedCoverageData);

		int mainLine = TestCoverageConsole::GetTestCoverageConsoleCppMainLine();
		TestLine(file, mainLine + 15, false); // TestThrowHandledException
		TestLine(file, mainLine + 17, true); // TestThrowUnHandledCppException
		TestLine(file, mainLine + 19, true); // TestThrowUnHandledSEHException
		TestLine(file, mainLine + 21, false); // TestBreakPoint
		TestLine(file, mainLine + 23, true); // TestChildProcess
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, TestFileInSeveralModules)
	{
		auto coverageData = ComputeCoverageDataPatterns(
			{ TestCoverageConsole::TestFileInSeveralModules },
			{ TestCoverageSharedLib::GetOutputBinaryPath().wstring(), TestCoverageConsole::GetOutputBinaryPath().wstring()},
			{ TestCoverageSharedLib::GetSharedFunctionFilename().wstring() });

		const auto sharedFunctionLine = TestCoverageSharedLib::GetSharedFunctionLine();

		const auto& fileFromFirstModule = *coverageData.GetModules().at(0)->GetFiles().at(0);
		TestLine(fileFromFirstModule, sharedFunctionLine + 3, false);
		TestLine(fileFromFirstModule, sharedFunctionLine + 4, true);

		const auto& fileFromSecondModule = *coverageData.GetModules().at(1)->GetFiles().at(0);
		TestLine(fileFromSecondModule, sharedFunctionLine + 3, true);
		TestLine(fileFromSecondModule, sharedFunctionLine + 4, false);

		cov::CoverageDataMerger().MergeFileCoverage(coverageData);
		for (const auto* file : { &fileFromFirstModule, &fileFromSecondModule })
		{
			TestLine(*file, sharedFunctionLine + 3, true);
			TestLine(*file, sharedFunctionLine + 4, true);
		}
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, SpecialLineInfo)
	{
		const auto specialLineInfoFilename = TestCoverageConsole::GetSpecialLineInfoFilename().wstring();

		auto coverageData = ComputeCoverageData(
			TestCoverageConsole::TestSpecialLineInfo, specialLineInfoFilename);
		auto& file = GetFirstFileCoverage(coverageData);
		ASSERT_TRUE(boost::algorithm::iequals(specialLineInfoFilename, file.GetPath().filename().wstring()));
		for (const auto& lineInfo : file.GetLines())
			ASSERT_TRUE(lineInfo.HasBeenExecuted());
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, UnifiedDiff)
	{
		const auto modulePattern = TestCoverageConsole::GetOutputBinaryPath().wstring();
		const auto sourcePattern = TestCoverageConsole::GetMainCppFilename().wstring();
		std::vector<cov::UnifiedDiffSettings> unifiedDiffSettingsCollection;
		auto diffPath = boost::filesystem::path(PROJECT_DIR) / "Data" / "TestCoverageConsole.diff";

		unifiedDiffSettingsCollection.push_back({diffPath, boost::none});
		auto coverageData = ComputeCoverageData(
			{ TestCoverageConsole::TestBasic }, 
			modulePattern, sourcePattern, 
			unifiedDiffSettingsCollection);
		const auto& file = GetFirstFileCoverage(coverageData);
		ASSERT_EQ(3, file.GetLines().size());
		int mainLine = TestCoverageConsole::GetTestCoverageConsoleCppMainLine();
		TestLine(file, mainLine + 2, true);
		TestLine(file, mainLine + 29, false);
		TestLine(file, mainLine + 31, true);
	}
}