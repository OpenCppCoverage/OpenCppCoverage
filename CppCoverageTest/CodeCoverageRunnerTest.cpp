#include "stdafx.h"

#include <boost/filesystem.hpp>
#include <boost/make_shared.hpp>

#include <boost/log/trivial.hpp>

#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/CoverageSettings.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"
#include "CppCoverage/Patterns.hpp"
#include "CppCoverage/CppCoverageLog.hpp"

#include "Tools.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;
namespace logging = boost::log;

namespace CppCoverageTest
{

	class CodeCoverageRunnerTest : public ::testing::Test
	{
	public:
		CodeCoverageRunnerTest()
			: exePath_(fs::path{ Tools::GetConsoleForCppCoverageTestPath() })
		{
			startInfo_.reset(new cov::StartInfo(exePath_.wstring()));
		}
		
		//---------------------------------------------------------------------
		void SetUp() override
		{
			error_ = boost::make_shared<std::ostringstream>();

			CppCoverage::SetLoggerMinSeverity(logging::trivial::error);
			CppCoverage::InitLoggerOstream(error_);
		}

		//---------------------------------------------------------------------
		void TearDown() override
		{
			CppCoverage::ResetLogger();
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
			const std::wstring& modulePattern,
			const std::wstring& sourcePattern)
		{
			cov::CodeCoverageRunner codeCoverageRunner;
			cov::Patterns modulePatterns{false};
			cov::Patterns sourcePatterns{ false };
			cov::CoverageSettings coverageSettings{modulePatterns, sourcePatterns};

			modulePatterns.AddSelectedPatterns(modulePattern);
			sourcePatterns.AddSelectedPatterns(sourcePattern);		

			return codeCoverageRunner.RunCoverage(*startInfo_, coverageSettings);
		}

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

		std::string GetError() const
		{
			return error_->str();
		}

		cov::StartInfo& GetStartInfo() const
		{
			return *startInfo_;
		}

	private:
		fs::path exePath_;
		boost::shared_ptr<std::ostringstream> error_;
		std::unique_ptr<cov::StartInfo> startInfo_;
	};

	TEST_F(CodeCoverageRunnerTest, RunCoverage)
	{		
		cov::CoverageData coverageData = ComputeCoverageData(
			L".*" + Tools::GetConsoleForCppCoverageTestFilename(), L".*CppCoverage.*.cpp");
		cov::FileCoverage* file;
		GetFileCoverage(coverageData, file);

		TestLine(*file, 23, true);
		TestLine(*file, 24, true);
		ASSERT_TRUE((*file)[25] == nullptr);
		TestLine(*file, 26, false);
	}

	TEST_F(CodeCoverageRunnerTest, RunCoverageDll)
	{	
		GetStartInfo().AddArguments(L"42"); 
		cov::CoverageData coverageData = ComputeCoverageData(
			L".*ForCppCoverageTest.*", L".*CppCoverage.*.cpp");

		const auto& modules = coverageData.GetModules();
		ASSERT_EQ(2, modules.size());		

		const auto& files = modules[1]->GetFiles();
		ASSERT_EQ(1, files.size());
		const auto& file = *files[0];

		TestLine(file, 10, true);
		TestLine(file, 11, true);
		TestLine(file, 12, true);
		ASSERT_TRUE(file[13] == nullptr);
		TestLine(file, 14, false);		
	}
	
	TEST_F(CodeCoverageRunnerTest, HandledException)
	{
		GetStartInfo().AddArguments(L"ThrowHandledException");
		ComputeCoverageData(L".*", L".*");		
		ASSERT_EQ(std::string::npos, 
			GetError().find(cov::CodeCoverageRunner::unhandledExceptionErrorMessage));
	}
	
	TEST_F(CodeCoverageRunnerTest, NotHandledException)
	{
		GetStartInfo().AddArguments(L"ThrowUnHandledException");
		ComputeCoverageData(L".*", L".*");
		ASSERT_NE(std::string::npos, 
			GetError().find(cov::CodeCoverageRunner::unhandledExceptionErrorMessage));
	}

}