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
#include "Tools/Log.hpp"
#include "Tools/Tool.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"
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
		CodeCoverageRunnerTest()
			: exePath_(TestCoverageConsole::GetOutputBinaryPath())
		{
			startInfo_.reset(new cov::StartInfo(exePath_.wstring()));
		}
		
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

			return codeCoverageRunner.RunCoverage(*startInfo_, coverageSettings);
		}

		//---------------------------------------------------------------------
		void GetFileCoverage(
			const cov::CoverageData& coverageData,
			cov::FileCoverage*& fileCoverage) // boost test does not support non void function
		{
			const auto& modules = coverageData.GetModules();
			ASSERT_EQ(1, modules.size());

			const auto& module = *modules[0];
			ASSERT_EQ(exePath_, module.GetPath());

			const auto& files = module.GetFiles();
			ASSERT_EQ(1, files.size());

			fileCoverage = files[0].get();
		}

		//---------------------------------------------------------------------
		std::string GetError() const
		{
			return error_->str();
		}

		//---------------------------------------------------------------------
		cov::StartInfo& GetStartInfo() const
		{
			return *startInfo_;
		}

	private:
		fs::path exePath_;
		boost::shared_ptr<std::ostringstream> error_;
		std::unique_ptr<cov::StartInfo> startInfo_;
	};

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, RunCoverage)
	{		
		cov::CoverageData coverageData = ComputeCoverageData(
			TestCoverageConsole::GetOutputBinaryPath().wstring(),
			TestCoverageConsole::GetMainCppPath().wstring());
		cov::FileCoverage* file;
		GetFileCoverage(coverageData, file);

		int line = 30;
		TestLine(*file, line++, true);
		TestLine(*file, line++, true);
		ASSERT_TRUE((*file)[line++] == nullptr);
		TestLine(*file, line++, false);
		ASSERT_EQ(0, coverageData.GetExitCode());
	}

	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, RunCoverageDll)
	{			
		GetStartInfo().AddArguments(TestCoverageConsole::TestSharedLib);
		cov::CoverageData coverageData = ComputeCoverageData(			
			TestCoverageSharedLib::GetOutputBinaryPath().wstring(),
			TestCoverageSharedLib::GetMainCppPath().wstring());

		const auto& modules = coverageData.GetModules();
		ASSERT_EQ(1, modules.size());		

		const auto& files = modules[0]->GetFiles();
		ASSERT_EQ(1, files.size());
		const auto& file = *files[0];
		int line = 15;
		TestLine(file, line++, true);
		TestLine(file, line++, true);
		TestLine(file, line++, true);
		ASSERT_TRUE(file[line++] == nullptr);
		TestLine(file, line++, false);
	}
	
	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, HandledException)
	{
		GetStartInfo().AddArguments(TestCoverageConsole::TestThrowHandledException);
		cov::CoverageData coverageData = ComputeCoverageData(L"*", L"*");
		ASSERT_EQ(std::string::npos, 
			GetError().find(cov::CodeCoverageRunner::unhandledExceptionErrorMessage));
		ASSERT_EQ(0, coverageData.GetExitCode());
	}
	
	//-------------------------------------------------------------------------
	TEST_F(CodeCoverageRunnerTest, NotHandledException)
	{
		GetStartInfo().AddArguments(TestCoverageConsole::TestThrowUnHandledException);
		cov::CoverageData coverageData = ComputeCoverageData(L"*", L"*");
		ASSERT_NE(std::string::npos, 
			GetError().find(cov::CodeCoverageRunner::unhandledExceptionErrorMessage));
		ASSERT_NE(0, coverageData.GetExitCode());
	}
}