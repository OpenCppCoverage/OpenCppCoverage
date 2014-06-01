#include "stdafx.h"

#include <boost/filesystem.hpp>

#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "OpenCppCoverage/OpenCppCoverage.hpp"
#include "CppCoverage/OptionsParser.hpp"

#include "Tools/ScopedAction.hpp"
#include "Tools/Tool.hpp"
#include "Tools/TemporaryPath.hpp"

#include "OpenCppCoverageTestTools.hpp"

namespace fs = boost::filesystem;
namespace cov = CppCoverage;

namespace OpenCppCoverageTest
{	
	namespace
	{
		//---------------------------------------------------------------------
		void CheckOutputDirectory(const Tools::TemporaryPath& tempFolder)
		{			
			ASSERT_FALSE(fs::is_empty(tempFolder.GetPath()));
		}

		//---------------------------------------------------------------------
		int RunCoverageOnProgram(
			const fs::path& programToRun,
			const std::vector<std::wstring>& arguments)
		{
			Tools::TemporaryPath tempFolder;

			std::vector<std::pair<std::string, std::string>> coverageArguments{
				{ cov::OptionsParser::SelectedModulesOption, programToRun.string() },
				{ cov::OptionsParser::SelectedSourcesOption, SOLUTION_DIR } };

			int exitCode = RunCoverageOn(coverageArguments, tempFolder, programToRun, arguments);
			
			CheckOutputDirectory(tempFolder);
			return exitCode;
		}
	}

	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, Basic)
	{		
		fs::path testCoverageConsole = TestCoverageConsole::GetOutputBinaryPath();
							
		ASSERT_EQ(0, RunCoverageOnProgram(testCoverageConsole, {}));
	}	

	
	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, UnhandledException)
	{
		fs::path testCoverageConsole = TestCoverageConsole::GetOutputBinaryPath();

		ASSERT_NE(0, RunCoverageOnProgram(testCoverageConsole, { TestCoverageConsole::TestThrowUnHandledException }));
	}

	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, CppCoverageTest)
	{
		fs::path cppCoverageTest{ OUT_DIR };
		
		cppCoverageTest /= "CppCoverageTest.exe";	
		ASSERT_EQ(0, RunCoverageOnProgram(cppCoverageTest, {}));
	}

	//-------------------------------------------------------------------------
	TEST(OpenCppCoverageConsoleTest, ExporterTest)
	{
		fs::path exporterTest{ OUT_DIR };
		
		exporterTest /= "ExporterTest.exe";
		ASSERT_EQ(0, RunCoverageOnProgram(exporterTest, {}));
	}	
}