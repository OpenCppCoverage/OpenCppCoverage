#include "stdafx.h"

#include <boost/filesystem.hpp>

#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/CoverageSettings.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"
#include "Tools.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace CppCoverageTest
{

	namespace
	{
		//---------------------------------------------------------------------
		void TestLine(const cov::FileCoverage& file, unsigned int lineNumber, bool hasBeenExecuted)
		{
			const auto* line = file[lineNumber];

			ASSERT_NE(nullptr, line);
			ASSERT_EQ(hasBeenExecuted, line->HasBeenExecuted());
		}

		//---------------------------------------------------------------------
		cov::CoverageData ComputeCoverageData(
			const cov::StartInfo& startInfo,
			const std::wstring& modulePattern,
			const std::wstring& sourcePattern)
		{
			cov::CodeCoverageRunner codeCoverageRunner;
			cov::CoverageSettings coverageSettings;

			coverageSettings.SetIsRegexCaseSensitiv(false);
			coverageSettings.AddModulePositivePatterns(modulePattern);
			coverageSettings.AddSourcePositivePatterns(sourcePattern); 

			return codeCoverageRunner.RunCoverage(startInfo, coverageSettings);
		}

		void GetFileCoverage(
			const fs::path& modulePath,
			const cov::CoverageData& coverageData,
			cov::FileCoverage*& fileCoverage) // boost test does not support non void function
		{
			const auto& modules = coverageData.GetModules();
			ASSERT_EQ(1, modules.size());

			const auto& module = *modules[0];
			ASSERT_EQ(modulePath, module.GetPath());

			const auto& files = module.GetFiles();
			ASSERT_EQ(1, files.size());

			fileCoverage = files[0].get();
		}
	}

	TEST(CodeCoverageRunnerTest, RunCoverage)
	{		
		auto path = fs::path{Tools::GetConsoleForCppCoverageTestPath()};
		cov::StartInfo startInfo{path.wstring()};

		cov::CoverageData coverageData = ComputeCoverageData(startInfo, L".*" + Tools::GetConsoleForCppCoverageTestFilename(), L".*CppCoverage.*.cpp"); // $$ if we have cpp we have memory pb skip some dll $$ test with ConsoleForCppCoverageTest.cpp
		cov::FileCoverage* file;
		GetFileCoverage(path, coverageData, file);

		TestLine(*file, 12, true);
		TestLine(*file, 13, true);
		ASSERT_TRUE((*file)[14] == nullptr);
		TestLine(*file, 15, false);
	}

	TEST(CodeCoverageRunnerTest, RunCoverageDll)
	{
		auto path = fs::path{ Tools::GetConsoleForCppCoverageTestPath() };
		cov::StartInfo startInfo{ path.wstring() };

		startInfo.AddArguments(L"42"); 
		cov::CoverageData coverageData = ComputeCoverageData(startInfo, L".*ForCppCoverageTest.*", L".*CppCoverage.*.cpp");

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
}