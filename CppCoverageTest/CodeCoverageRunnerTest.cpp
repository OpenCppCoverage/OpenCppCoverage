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
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/count_if.hpp>

#include <boost/log/trivial.hpp>

#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/StartInfo.hpp"
#include "Plugin/Exporter/CoverageData.hpp"
#include "CppCoverage/CoverageFilterSettings.hpp"
#include "Plugin/Exporter/ModuleCoverage.hpp"
#include "Plugin/Exporter/FileCoverage.hpp"
#include "Plugin/Exporter/LineCoverage.hpp"
#include "CppCoverage/Patterns.hpp"
#include "CppCoverage/ExceptionHandler.hpp"
#include "CppCoverage/CoverageDataMerger.hpp"
#include "CppCoverage/UnifiedDiffSettings.hpp"
#include "CppCoverage/RunCoverageSettings.hpp"

#include "Tools/Log.hpp"
#include "Tools/Tool.hpp"

#include "TestHelper/CoverageDataComparer.hpp"
#include "TestHelper/Tools.hpp"
#include "TestHelper/TemporaryPath.hpp"

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
namespace fs = std::filesystem;
namespace logging = boost::log;

namespace CppCoverageTest
{
	using TestTools::CoverageArgs;
	using TestTools::ComputeCoverageData;
	using TestTools::ComputeCoverageDataPatterns;

	//-------------------------------------------------------------------------
	class CodeCoverageRunnerTest : public TestTools::CoverageLevelTest
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
		void TestLine(const Plugin::FileCoverage& file, unsigned int lineNumber, bool hasBeenExecuted)
		{
			const auto* line = file[lineNumber];

			ASSERT_NE(nullptr, line);
			ASSERT_EQ(hasBeenExecuted, line->HasBeenExecuted());
		}

		//---------------------------------------------------------------------
		Plugin::CoverageData ComputeCoverageDataInBothMode(
			const std::wstring& programArg,
			std::wstring modulePattern,
			std::wstring sourcePattern,
			bool continueAfterCppException = false)
		{
			std::vector<std::wstring> arguments = { programArg };
			CoverageArgs args{ arguments, modulePattern, sourcePattern };

			args.continueAfterCppException_ = continueAfterCppException;

			auto coverageData = ComputeCoverageDataPatterns(args, coverageLevel_);

			// Run child child process.
			args.arguments_.insert(args.arguments_.begin(), TestCoverageConsole::TestChildProcess);
			auto coverageDataChildProcess = ComputeCoverageDataPatterns(args, coverageLevel_);

			if (!TestHelper::CoverageDataComparer().IsFirstCollectionContainsSecond(
					coverageDataChildProcess.GetModules(), coverageData.GetModules()))
			{
				throw std::runtime_error("Invalid coverage");
			}
			return coverageData;
		}

		//---------------------------------------------------------------------
		Plugin::CoverageData RunCoverageWithException(
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
		Plugin::CoverageData ComputeCoverageData(const std::wstring& programArg, const std::wstring& fileArgument)
		{
			return ComputeCoverageDataInBothMode(
				programArg,
				TestCoverageConsole::GetOutputBinaryPath().filename().wstring(),
				fileArgument);
		}

		//---------------------------------------------------------------------
		Plugin::FileCoverage& GetFirstFileCoverage(const Plugin::CoverageData& coverageData)
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
		intptr_t CountExecutedLines(const Plugin::FileCoverage& file)
		{
			auto lines = file.GetLines();
			return boost::count_if(lines,
				[](const auto& line) { return line.HasBeenExecuted(); });
		}

		//-------------------------------------------------------------------------
		void ValidateCoverageLevel(const std::wstring& filename, const Plugin::FileCoverage& file, std::function<void()> lineLevelValidator)
		{
			if (coverageLevel_ == cov::CoverageLevel::Line)
			{
				lineLevelValidator();
			}
			else
			{
				ASSERT_EQ(0, file.GetLines().size());
				ASSERT_EQ(filename, file.GetPath().filename().wstring());
			}			
		}

	private:
		boost::shared_ptr<std::ostringstream> error_;
	};

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, RunCoverage)
	{		
		const auto testBasicFilename = TestCoverageConsole::GetTestBasicFilename().wstring();
		
		Plugin::CoverageData coverageData = ComputeCoverageData(TestCoverageConsole::TestBasic, testBasicFilename);
		auto& file = GetFirstFileCoverage(coverageData);

		ValidateCoverageLevel(testBasicFilename, file, [&]() {
			int line = TestCoverageConsole::GetTestBasicLine() + 1;
			TestLine(file, line++, true);
			TestLine(file, line++, true);
			ASSERT_TRUE(file[line++] == nullptr);
			TestLine(file, line++, false);
			ASSERT_EQ(0, coverageData.GetExitCode());
		});
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, RunCoverageDll)
	{
		auto mainSharedLibFile = TestCoverageSharedLib::GetMainCppPath().wstring();

		Plugin::CoverageData coverageData = ComputeCoverageDataInBothMode(TestCoverageConsole::TestSharedLib,
			TestCoverageSharedLib::GetOutputBinaryPath().wstring(), mainSharedLibFile);
		auto& file = GetFirstFileCoverage(coverageData);

		ValidateCoverageLevel(TestCoverageSharedLib::GetMainCppPath().filename().wstring(), file, [&]() {
			int line = 31;
			TestLine(file, line++, true);
			TestLine(file, line++, true);
			TestLine(file, line++, true);
			ASSERT_EQ(nullptr, file[line++]);
			TestLine(file, line++, false);
		});
	}
	
	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, RunThread)
	{
		const auto filter = TestCoverageConsole::GetTestThreadFilename().wstring();

		Plugin::CoverageData coverageData = ComputeCoverageData(TestCoverageConsole::TestThread, filter);
		auto& file = GetFirstFileCoverage(coverageData);
		
		ValidateCoverageLevel(filter, file, [&]() {
			int line = 28;
			TestLine(file, line++, true);
			ASSERT_EQ(nullptr, file[line++]);
			TestLine(file, line++, true);
			TestLine(file, line++, true);
			TestLine(file, line++, true);
		});
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, HandledException)
	{
		Plugin::CoverageData coverageData = RunCoverageWithException(
			TestCoverageConsole::TestThrowHandledException, false);
		ASSERT_EQ(std::string::npos, 
			GetError().find(cov::ExceptionHandler::UnhandledExceptionErrorMessage));
		ASSERT_EQ(0, coverageData.GetExitCode());
	}
	
	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, UnHandledSEHException)
	{
		Plugin::CoverageData coverageData = RunCoverageWithException(
			TestCoverageConsole::TestThrowUnHandledSEHException, false);
		ASSERT_NE(std::string::npos, 
			GetError().find(cov::ExceptionHandler::UnhandledExceptionErrorMessage));
		ASSERT_NE(0, coverageData.GetExitCode());
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, UnHandledCppException)
	{
		auto coverageData = RunCoverageWithException(
			TestCoverageConsole::TestThrowUnHandledCppException, false);
		ASSERT_NE(std::string::npos,
			GetError().find(cov::ExceptionHandler::UnhandledExceptionErrorMessage));
		ASSERT_EQ(cov::ExceptionHandler::CppExceptionErrorCode, coverageData.GetExitCode());
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, UnHandledCppExceptionContinue)
	{
		auto coverageData = RunCoverageWithException(
			TestCoverageConsole::TestThrowUnHandledCppException, true);
		auto& file = GetFirstFileCoverage(coverageData);
		
		ValidateCoverageLevel(TestCoverageConsole::GetMainCppFilename().filename().wstring(), file, [&]() {
			auto returnLine = TestCoverageConsole::GetTestCoverageConsoleCppMainReturnLine();
			TestLine(file, returnLine, true);
		});
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, UnHandledCppExceptionNotContinue)
	{
		auto coverageData = RunCoverageWithException(
			TestCoverageConsole::TestThrowUnHandledCppException, false);
		auto& file = GetFirstFileCoverage(coverageData);
		auto returnLine = TestCoverageConsole::GetTestCoverageConsoleCppMainReturnLine();

		ValidateCoverageLevel(TestCoverageConsole::GetMainCppFilename().filename().wstring(), file, [&]() {
			TestLine(file, returnLine, false);
		});
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, ChildProcess)
	{
		std::vector<std::wstring> arguments = {
			TestCoverageConsole::TestChildProcess,
			TestCoverageConsole::TestThrowHandledException };
		const auto modulePattern = TestCoverageConsole::GetOutputBinaryPath().wstring();
		const auto sourcePattern = TestCoverageConsole::GetMainCppFilename().wstring();

		CoverageArgs args{ arguments, modulePattern, sourcePattern };

		args.coverChildren_ = true;
		auto rootAndChildProcess = ComputeCoverageDataPatterns(args, coverageLevel_);

		args.coverChildren_ = false;
		auto rootProcessOnly = ComputeCoverageDataPatterns(args, coverageLevel_);

		const auto& rootOnlyModules = rootProcessOnly.GetModules();
		const auto& rootAndChildModules = rootAndChildProcess.GetModules();

		auto filename = TestCoverageConsole::GetMainCppFilename().wstring();
		{
			auto& file = GetFirstFileCoverage(rootAndChildProcess);
			ValidateCoverageLevel(filename, file, [&]() {
				ASSERT_TRUE(TestHelper::CoverageDataComparer().IsFirstCollectionContainsSecond(
					rootAndChildModules, rootOnlyModules));
			});
		}
		{
			auto& file = GetFirstFileCoverage(rootProcessOnly);
			ValidateCoverageLevel(filename, file, [&]() {
				ASSERT_FALSE(TestHelper::CoverageDataComparer().IsFirstCollectionContainsSecond(
					rootOnlyModules, rootAndChildModules));
			});
		}
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, SeveralChildProcess)
	{
		auto filename = TestCoverageConsole::GetMainCppFilename().wstring();
		auto coverageData = TestTools::ComputeCoverageData(
			{	TestCoverageConsole::TestChildProcess,
				TestCoverageConsole::TestThrowUnHandledCppException,
				TestCoverageConsole::TestThrowUnHandledSEHException },
			TestCoverageConsole::GetOutputBinaryPath().wstring(),
			filename,
			coverageLevel_);

		std::vector<Plugin::CoverageData> coverageDataCollection;
		coverageDataCollection.push_back(std::move(coverageData));
		auto mergedCoverageData = cov::CoverageDataMerger().Merge(coverageDataCollection);

		auto& file = GetFirstFileCoverage(mergedCoverageData);

		ValidateCoverageLevel(filename, file, [&]() {
			int mainLine = TestCoverageConsole::GetTestCoverageConsoleCppMainStartLine();
			TestLine(file, mainLine + 15, false); // TestThrowHandledException
			TestLine(file, mainLine + 17, true); // TestThrowUnHandledCppException
			TestLine(file, mainLine + 19, true); // TestThrowUnHandledSEHException
			TestLine(file, mainLine + 21, false); // TestBreakPoint
			TestLine(file, mainLine + 23, true); // TestChildProcess
			
		});
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, TestFileInSeveralModules)
	{
		CoverageArgs args{
			{ TestCoverageConsole::TestFileInSeveralModules },
			{ TestCoverageSharedLib::GetOutputBinaryPath().wstring() },
			{ TestCoverageSharedLib::GetSharedFunctionFilename().wstring() }
		};

		args.modulePatternCollection_.push_back(TestCoverageConsole::GetOutputBinaryPath().wstring());
		auto coverageData = ComputeCoverageDataPatterns(args, coverageLevel_);
		const auto sharedFunctionLine = TestCoverageSharedLib::GetSharedFunctionLine();

		const auto& fileFromFirstModule = *coverageData.GetModules().at(0)->GetFiles().at(0);
		const auto& fileFromSecondModule = *coverageData.GetModules().at(1)->GetFiles().at(0);
		auto filename = TestCoverageSharedLib::GetSharedFunctionFilename().filename().wstring();
		{
			ValidateCoverageLevel(filename, fileFromFirstModule, [&]() {
				TestLine(fileFromFirstModule, sharedFunctionLine + 3, false);
				TestLine(fileFromFirstModule, sharedFunctionLine + 4, true);
			});
		}
		{
			auto filename = fileFromSecondModule.GetPath().filename().wstring();
			ValidateCoverageLevel(filename, fileFromSecondModule, [&]() {
				TestLine(fileFromSecondModule, sharedFunctionLine + 3, true);
				TestLine(fileFromSecondModule, sharedFunctionLine + 4, false);
			});
		}

		if (coverageLevel_ == cov::CoverageLevel::Line)
		{
			cov::CoverageDataMerger().MergeFileCoverage(coverageData);
			for (const auto* file : { &fileFromFirstModule, &fileFromSecondModule })
			{
				TestLine(*file, sharedFunctionLine + 3, true);
				TestLine(*file, sharedFunctionLine + 4, true);
			}
		}
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, SpecialLineInfo)
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
	TEST_P(CodeCoverageRunnerTest, UnifiedDiff)
	{
		auto diffPath = std::filesystem::path(PROJECT_DIR) / "Data" / "TestDiff.diff";

		CoverageArgs args{ 
			{ TestCoverageConsole::TestDiff },
			TestCoverageConsole::GetOutputBinaryPath().wstring(),
			TestCoverageConsole::GetTestDiffFilename().wstring() };
		args.unifiedDiffSettingsCollection_.push_back({ diffPath, boost::none });
		auto coverageData = ComputeCoverageDataPatterns(args, coverageLevel_);

		const auto& file = GetFirstFileCoverage(coverageData);	
		ValidateCoverageLevel(TestCoverageConsole::GetTestDiffFilename().filename().wstring(), file, [&]() {
			ASSERT_EQ(4, file.GetLines().size());

			TestLine(file, 25, true);
			TestLine(file, 26, true);
			TestLine(file, 27, true);
			TestLine(file, 28, false);
		});
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, SpecialChars)
	{
		const auto fileWithSpecialChars = TestCoverageConsole::GetFileWithSpecialChars();
		CoverageArgs args{ 
			{ L"" },
			TestCoverageConsole::GetOutputBinaryPath().filename().wstring(),
			fileWithSpecialChars.wstring() };

		auto coverageData = ComputeCoverageDataPatterns(args, coverageLevel_);
		const auto& file = GetFirstFileCoverage(coverageData);
		ASSERT_TRUE(boost::algorithm::iequals(fileWithSpecialChars.wstring(), file.GetPath().filename().wstring()));
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, UnloadReloadDll)
	{
		const auto filename = TestHelper::GetTestUnloadDllFilename().wstring();

		auto coverageData = ComputeCoverageDataInBothMode(
			TestCoverageConsole::TestUnloadReloadDll, 
			TestHelper::GetOutputBinaryPath().wstring(), 
			filename);
		const auto& file = GetFirstFileCoverage(coverageData);
		ValidateCoverageLevel(filename, file, [&]() {
			ASSERT_TRUE(boost::algorithm::iequals(filename, file.GetPath().filename().wstring()));
			ASSERT_NE(0, CountExecutedLines(file));
		});
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, OptimizedBuild)
	{
		CoverageArgs args{{}, L"OptimizedBuildVS2013", L"OptimizedBuildVS2013"};
		auto optimizedBuildProjectPath = fs::path{ PROJECT_DIR } / "OptimizedBuildVS2013";
		args.programToRun_ = optimizedBuildProjectPath /
		                     "Release" / "OptimizedBuildVS2013.exe";

		if (!Tools::FileExists(args.programToRun_))
		{
			throw std::runtime_error(
				"You need to compile " + optimizedBuildProjectPath.string() +
				" in release mode with Visual Studio 2013 to run this test.");
		}

		auto computeCoverage = [&](bool optimizedBuild) {
			args.optimizedBuildSupport_ = optimizedBuild;
			return ComputeCoverageDataPatterns(args, coverageLevel_);
		};

		auto coverageData = computeCoverage(false);
		ASSERT_NE(0, coverageData.GetExitCode());

		auto coverageDataOptimizedBuild = computeCoverage(true);
		ASSERT_EQ(0, coverageDataOptimizedBuild.GetExitCode());

		const auto& fileOptimizedBuild =
			GetFirstFileCoverage(coverageDataOptimizedBuild);
		const auto& file = GetFirstFileCoverage(coverageData);
		auto optimizedBuildCount = CountExecutedLines(fileOptimizedBuild);
		auto count = CountExecutedLines(file);

		ASSERT_GT(optimizedBuildCount, count);
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, ExcludedLine)
	{
		CoverageArgs args{
			{ TestCoverageConsole::TestBasic },
			TestCoverageConsole::GetOutputBinaryPath().wstring(),
			TestCoverageConsole::GetTestBasicFilename().wstring()
		};

		auto computeCoverage = [&](const std::vector<std::wstring>& excludedRegexes)
		{
			args.excludedLineRegexes_ = excludedRegexes;
			return ComputeCoverageDataPatterns(args, coverageLevel_);
		};
		auto coverageDataWithExcludedLine = computeCoverage({ L".*ExcludedLine.*" });
		const auto& fileWithExcludedLine = GetFirstFileCoverage(coverageDataWithExcludedLine);

		auto coverageData = computeCoverage({});
		const auto& file = GetFirstFileCoverage(coverageData);
		
		ValidateCoverageLevel(TestCoverageConsole::GetTestBasicFilename().filename().wstring(), file, [&]() {
			ASSERT_EQ(file.GetLines().size(), fileWithExcludedLine.GetLines().size() + 1);
		});
	}

	//-------------------------------------------------------------------------
	TEST_P(CodeCoverageRunnerTest, SubstitutePdbSourcePath)
	{
		const auto filename = TestCoverageConsole::GetMainCppFilename();
		CoverageArgs args{
		    {},
		    TestCoverageConsole::GetOutputBinaryPath().filename().wstring(),
		    filename.wstring()};
		auto originalCoverageData = ComputeCoverageDataPatterns(args, coverageLevel_);
		auto originalPath =
			GetFirstFileCoverage(originalCoverageData).GetPath();

		ASSERT_EQ(filename, originalPath.filename());

		TestHelper::TemporaryPath folder{
		    TestHelper::TemporaryPathOption::CreateAsFolder};
		const auto expectedPath = folder.GetPath() / filename;

		args.sourcePatternCollection_.clear();
		args.sourcePatternCollection_.push_back(expectedPath.wstring());
		args.substitutePdbSourcePath_.push_back(
		    {originalPath.parent_path().wstring(), folder->wstring()});
		auto coverageData = ComputeCoverageDataPatterns(args, coverageLevel_);
		const auto& file = GetFirstFileCoverage(coverageData);
		ASSERT_EQ(expectedPath, file.GetPath());
	}

	INSTANTIATE_TEST_SUITE_P(
		CodeCoverageRunnerTests,
		CodeCoverageRunnerTest,
		TestTools::CoverageLevelValues);
}