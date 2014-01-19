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

	void TestLine(const cov::FileCoverage& file, unsigned int lineNumber, bool hasBeenExecuted)
	{
		const auto* line = file[lineNumber];

		ASSERT_NE(nullptr, line);
		ASSERT_EQ(hasBeenExecuted, line->HasBeenExecuted());
	}

	TEST(CodeCoverageRunnerTest, RunCoverage)
	{
		cov::CodeCoverageRunner codeCoverageRunner;
		auto path = fs::path{Tools::GetConsoleForCppCoverageTestPath()};
		cov::StartInfo startInfo{path.wstring()};
		cov::CoverageSettings coverageSettings;

		coverageSettings.SetIsRegexCaseSensitiv(false);
		coverageSettings.AddModulePositivePatterns(L".*" + Tools::GetConsoleForCppCoverageTestFilename());
		coverageSettings.AddSourcePositivePatterns(L".*ConsoleForCppCoverageTest.cpp"); // $$ if we have cpp we have memory pb skip some dll $$ test with ConsoleForCppCoverageTest.cpp

		cov::CoverageData coverageData = codeCoverageRunner.RunCoverage(startInfo, coverageSettings);

		const auto& modules = coverageData.GetModules();
		ASSERT_EQ(1, modules.size());

		const auto& module = *modules[0];
		ASSERT_EQ(path, module.GetName());

		const auto& files = module.GetFiles();
		ASSERT_EQ(1, files.size());

		const auto& file = *files[0];

		TestLine(file, 8, true);
		TestLine(file, 9, false);
		TestLine(file, 10, true);
	}
}