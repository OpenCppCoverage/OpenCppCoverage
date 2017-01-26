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
#include <boost/range/algorithm/count_if.hpp>

#include <boost/log/trivial.hpp>

#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/CoverageFilterSettings.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"
#include "CppCoverage/Patterns.hpp"
#include "CppCoverage/ExceptionHandler.hpp"
#include "CppCoverage/CoverageDataMerger.hpp"
#include "CppCoverage/UnifiedDiffSettings.hpp"
#include "CppCoverage/RunCoverageSettings.hpp"

#include "Tools/Log.hpp"
#include "Tools/Tool.hpp"

#include "TestHelper/CoverageDataComparer.hpp"
#include "TestHelper/Tools.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "TestCoverageConsole/TestBasic.hpp"
#include "TestCoverageConsole/TestThread.hpp"
#include "TestCoverageConsole/SpecialLineInfo.hpp"
#include "TestCoverageConsole/FileWithSpecialCharÈ‡Ë.hpp"
#include "TestCoverageConsole/TestDiff.hpp"

#include "TestCoverageSharedLib/TestCoverageSharedLib.hpp"
#include "TestCoverageOptimizedBuild/TestCoverageOptimizedBuild.hpp"

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
			bool coverChildren = true,
			bool continueAfterCppException = false,
			bool optimizedBuildSupport = false)
		{
			cov::CodeCoverageRunner codeCoverageRunner;
			cov::Patterns modulePatterns{false};
			cov::Patterns sourcePatterns{false};

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
			
			cov::CoverageFilterSettings coverageFilterSettings{modulePatterns, sourcePatterns};
			cov::StartInfo startInfo{ TestCoverageConsole::GetOutputBinaryPath().wstring() };

			for (const auto& argument: arguments)
				startInfo.AddArgument(argument);

			cov::RunCoverageSettings settings(
				startInfo,
				coverageFilterSettings,
				unifiedDiffSettingsCollection);
			settings.SetCoverChildren(coverChildren);
			settings.SetContinueAfterCppException(continueAfterCppException);
			settings.SetOptimizedBuildSupport(optimizedBuildSupport);

			auto coverageData = codeCoverageRunner.RunCoverage(settings);

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
			bool coverChildren = true,
			bool continueAfterCppException = false,
			bool optimizedBuildSupport = false)
		{
			return ComputeCoverageDataPatterns(
				arguments, 
				{ modulePattern }, 
				{ sourcePattern }, 
				unifiedDiffSettingsCollection, 
				coverChildren, 
				continueAfterCppException,
				optimizedBuildSupport);
		}

		//---------------------------------------------------------------------
		cov::CoverageData ComputeCoverageDataInBothMode(
			const std::wstring& programArg,
			std::wstring modulePattern,
			std::wstring sourcePattern,
			bool continueAfterCppException = false)
		{
			std::vector<std::wstring> arguments = { programArg };

			auto coverageData = ComputeCoverageData(
				arguments, 
				modulePattern, 
				sourcePattern, 
				{},
				true,
				continueAfterCppException);

			// Run child child process.
			arguments.insert(arguments.begin(), TestCoverageConsole::TestChildProcess);
			auto coverageDataChildProcess = ComputeCoverageData(
				arguments, 
				modulePattern, 
				sourcePattern, 
				{}, 
				true, 
				continueAfterCppException);

			if (!TestHelper::CoverageDataComparer().IsFirstCollectionContainsSecond(
					coverageDataChildProcess.GetModules(), coverageData.GetModules()))
			{
				throw std::runtime_error("Invalid coverage");
			}
			return coverageData;
		}

		//---------------------------------------------------------------------
		cov::CoverageData RunCoverageWithException(
			const std::wstring& programArg,
			bool continueAfterCppException)
		{
			return ComputeCoverageDataInBothMode(
				programArg,
				TestCoverageConsole::GetOutputBinaryPath().wstring(),
				TestCoverageConsole::GetMainCppFilename().wstring(),
				continueAfterCppException);
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
			return Tools::LocalToWString(error_->str());
		}

		//-------------------------------------------------------------------------
		intptr_t CountExecutedLines(const cov::FileCoverage& file)
		{
			auto lines = file.GetLines();
			return boost::count_if(lines,
				[](const auto& line) { return line.HasBeenExecuted(); });
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
		cov::CoverageData coverageData = RunCoverageWithException(
			TestCoverageConsole::TestThrowHandledException, false);
		ASSERT_EQ(std::string::npos, 
			GetError().find(cov::ExceptionHandler::UnhandledExceptionErrorMessage));
		ASSERT_EQ(0, coverageData.GetExitCode());
	}
	
	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, UnHandledSEHException)
	{
		cov::CoverageData coverageData = RunCoverageWithException(
			TestCoverageConsole::TestThrowUnHandledSEHException, false);
		ASSERT_NE(std::string::npos, 
			GetError().find(cov::ExceptionHandler::UnhandledExceptionErrorMessage));
		ASSERT_NE(0, coverageData.GetExitCode());
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, UnHandledCppException)
	{
		auto coverageData = RunCoverageWithException(
			TestCoverageConsole::TestThrowUnHandledCppException, false);
		ASSERT_NE(std::string::npos,
			GetError().find(cov::ExceptionHandler::UnhandledExceptionErrorMessage));
		ASSERT_EQ(cov::ExceptionHandler::CppExceptionErrorCode, coverageData.GetExitCode());
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, UnHandledCppExceptionContinue)
	{
		auto coverageData = RunCoverageWithException(
			TestCoverageConsole::TestThrowUnHandledCppException, true);
		auto& file = GetFirstFileCoverage(coverageData);
		auto returnLine = TestCoverageConsole::GetTestCoverageConsoleCppMainReturnLine();
		TestLine(file, returnLine, true);
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, UnHandledCppExceptionNotContinue)
	{
		auto coverageData = RunCoverageWithException(
			TestCoverageConsole::TestThrowUnHandledCppException, false);
		auto& file = GetFirstFileCoverage(coverageData);
		auto returnLine = TestCoverageConsole::GetTestCoverageConsoleCppMainReturnLine();
		TestLine(file, returnLine, false);
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

		int mainLine = TestCoverageConsole::GetTestCoverageConsoleCppMainStartLine();
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
		std::vector<cov::UnifiedDiffSettings> unifiedDiffSettingsCollection;
		auto diffPath = boost::filesystem::path(PROJECT_DIR) / "Data" / "TestDiff.diff";

		unifiedDiffSettingsCollection.push_back({diffPath, boost::none});
		auto coverageData = ComputeCoverageData(
			{ TestCoverageConsole::TestDiff }, 
			TestCoverageConsole::GetOutputBinaryPath().wstring(), 
			TestCoverageConsole::GetTestDiffFilename().wstring(),
			unifiedDiffSettingsCollection);
		const auto& file = GetFirstFileCoverage(coverageData);
		ASSERT_EQ(4, file.GetLines().size());
		
		TestLine(file, 27, true);
		TestLine(file, 28, true);
		TestLine(file, 29, true);
		TestLine(file, 30, false);
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, SpecialChars)
	{
		const auto fileWithSpecialChars = TestCoverageConsole::GetFileWithSpecialChars();

		auto coverageData = ComputeCoverageData(
			{L""},
			TestCoverageConsole::GetOutputBinaryPath().filename().wstring(),
			fileWithSpecialChars.wstring(), {}, true, false);
		const auto& file = GetFirstFileCoverage(coverageData);
		auto filename = file.GetPath().filename().wstring();
		ASSERT_TRUE(boost::algorithm::iequals(fileWithSpecialChars.wstring(), filename));
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, UnloadReloadDll)
	{
		const auto unloadDllFilename = TestHelper::GetTestUnloadDllFilename().wstring();

		auto coverageData = ComputeCoverageDataInBothMode(
			TestCoverageConsole::TestUnloadReloadDll, 
			TestHelper::GetOutputBinaryPath().wstring(), 
			unloadDllFilename);
		const auto& file = GetFirstFileCoverage(coverageData);
		auto filename = file.GetPath().filename().wstring();
		ASSERT_TRUE(boost::algorithm::iequals(unloadDllFilename, filename));
		ASSERT_NE(0, CountExecutedLines(file));
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, OptimizedBuild)
	{
		// This test works only on x86.
#ifndef _WIN64
		auto computeCoverage = [&](bool optimizedBuild)
		{
			return ComputeCoverageData(
				{ TestCoverageConsole::TestOptimizedBuild },
				TestCoverageOptimizedBuild::GetOutputBinaryPath().wstring(),
				TestCoverageOptimizedBuild::GetMainCppPath().wstring(), 
				{}, false, false, optimizedBuild);
		};

		auto coverageData = computeCoverage(false);
		ASSERT_NE(0, coverageData.GetExitCode());

		auto coverageDataOptimizedBuild = computeCoverage(true);
		ASSERT_EQ(0, coverageDataOptimizedBuild.GetExitCode());

		const auto& fileOptimizedBuild = GetFirstFileCoverage(coverageDataOptimizedBuild);
		const auto& file = GetFirstFileCoverage(coverageData);
		auto optimizedBuildCount = CountExecutedLines(fileOptimizedBuild);
		auto count = CountExecutedLines(file);

		ASSERT_GT(optimizedBuildCount, count);
#endif
	}
}