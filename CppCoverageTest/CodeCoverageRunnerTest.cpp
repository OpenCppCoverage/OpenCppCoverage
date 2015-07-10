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

#include "Tools/Log.hpp"
#include "Tools/Tool.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "TestCoverageConsole/TestBasic.hpp"
#include "TestCoverageConsole/TestThread.hpp"
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
		cov::CoverageData ComputeCoverageData(
			const std::wstring& programArg,
			std::wstring modulePattern,
			std::wstring sourcePattern)
		{
			cov::CodeCoverageRunner codeCoverageRunner;
			cov::Patterns modulePatterns{false};
			cov::Patterns sourcePatterns{false};
			
			cov::CoverageSettings coverageSettings{modulePatterns, sourcePatterns};

			boost::to_lower(modulePattern);
			boost::to_lower(sourcePattern);

			modulePatterns.AddSelectedPatterns(modulePattern);
			sourcePatterns.AddSelectedPatterns(sourcePattern);

			
			cov::StartInfo startInfo{ TestCoverageConsole::GetOutputBinaryPath().wstring() };
			startInfo.AddArgument(programArg);

			return codeCoverageRunner.RunCoverage(startInfo, coverageSettings);
		}

		//---------------------------------------------------------------------
		cov::CoverageData RunCoverageWithException(const std::wstring& programArg)
		{
			return ComputeCoverageData(
				programArg,
				TestCoverageSharedLib::GetOutputBinaryPath().wstring(),
				TestCoverageSharedLib::GetMainCppPath().wstring());
		}

		//---------------------------------------------------------------------
		cov::CoverageData ComputeCoverageData(const std::wstring& fileArgument)
		{
			return ComputeCoverageData(
				fileArgument,
				TestCoverageConsole::GetOutputBinaryPath().wstring(),
				fileArgument);
		}


		//---------------------------------------------------------------------
		void GetFirstFileCoverage(
			const cov::CoverageData& coverageData,			
			cov::FileCoverage*& fileCoverage) // boost test does not support non void function
		{
			const auto& modules = coverageData.GetModules();
			ASSERT_EQ(1, modules.size());

			const auto& module = *modules[0];			
			const auto& files = module.GetFiles();
			ASSERT_LT(0u, files.size());

			fileCoverage = files[0].get();
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
		const auto testBasic = TestCoverageConsole::GetTestBasicPath().wstring();
		
		cov::CoverageData coverageData = ComputeCoverageData(testBasic);
		cov::FileCoverage* file = nullptr;
		GetFirstFileCoverage(coverageData, file);

		int line = 26;
		ASSERT_NE(nullptr, file);
		TestLine(*file, line++, true);
		TestLine(*file, line++, true);
		ASSERT_TRUE((*file)[line++] == nullptr);
		TestLine(*file, line++, false);
		ASSERT_EQ(0, coverageData.GetExitCode());
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, RunCoverageDll)
	{			
		auto mainSharedLibFile = TestCoverageSharedLib::GetMainCppPath().wstring();

		cov::CoverageData coverageData = ComputeCoverageData(mainSharedLibFile, 
			TestCoverageSharedLib::GetOutputBinaryPath().wstring(), mainSharedLibFile);
		cov::FileCoverage* file = nullptr;
		GetFirstFileCoverage(coverageData, file);

		int line = 31;
		ASSERT_NE(nullptr, file);
		TestLine(*file, line++, true);
		TestLine(*file, line++, true);
		TestLine(*file, line++, true);
		ASSERT_EQ(nullptr, (*file)[line++]);
		TestLine(*file, line++, false);
	}
	
	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, RunThread)
	{
		const auto testThread = TestCoverageConsole::GetTestThreadPath().wstring();

		cov::CoverageData coverageData = ComputeCoverageData(testThread);
		cov::FileCoverage* file = nullptr;
		GetFirstFileCoverage(coverageData, file);

		int line = 28;
		ASSERT_NE(nullptr, file);

		TestLine(*file, line++, true);
		ASSERT_EQ(nullptr, (*file)[line++]);
		TestLine(*file, line++, true);
		TestLine(*file, line++, true);
		TestLine(*file, line++, true);				
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
}