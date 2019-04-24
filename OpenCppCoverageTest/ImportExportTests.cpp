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

#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "TestHelper/TemporaryPath.hpp"

#include "CppCoverage/ExportOptionParser.hpp"
#include "CppCoverage/ProgramOptions.hpp"
#include "Plugin/Exporter/CoverageData.hpp"
#include "CppCoverage/OptionsParser.hpp"

#include "Exporter/binary/CoverageDataDeserializer.hpp"

#include "TestHelper/CoverageDataComparer.hpp"

#include "OpenCppCoverageTestTools.hpp"

namespace fs = std::filesystem;
namespace cov = CppCoverage;

namespace OpenCppCoverageTest
{
	namespace
	{		
		
		//---------------------------------------------------------------------
		void RunCoverage(
			std::vector<std::pair<std::string, std::string>> coverageArguments,
			const TestHelper::TemporaryPath& output)
		{
			fs::path testCoverageConsole = TestCoverageConsole::GetOutputBinaryPath();						
			
			AddDefaultFilters(coverageArguments, testCoverageConsole);
			coverageArguments.emplace_back(cov::ProgramOptions::QuietOption, "");
			int exitCode = RunCoverageFor(coverageArguments, testCoverageConsole, {});
			
			ASSERT_EQ(0, exitCode);		
			ASSERT_TRUE(fs::exists(output));
		}
		
		//---------------------------------------------------------------------
		void RunCoverage(const std::string& exportType)
		{
			TestHelper::TemporaryPath output;
			std::vector<std::pair<std::string, std::string>> coverageArguments;

			coverageArguments.push_back(BuildExportTypeString(exportType, output));
			RunCoverage(coverageArguments, output);
		}

		//---------------------------------------------------------------------
		Plugin::CoverageData ReadCoverageDataFromFile(const TestHelper::TemporaryPath& temporaryPath)
		{
			Exporter::CoverageDataDeserializer coverageDataDeserializer;
			
			return coverageDataDeserializer.Deserialize(temporaryPath.GetPath(), "");
		}
	}

	//-------------------------------------------------------------------------
	TEST(ImportExportTest, ExportHtml)
	{
		RunCoverage(cov::ExportOptionParser::ExportTypeHtmlValue);
	}

	//-------------------------------------------------------------------------
	TEST(ImportExportTest, ExportCobertura)
	{
		RunCoverage(cov::ExportOptionParser::ExportTypeCoberturaValue);
	}

	//-------------------------------------------------------------------------
	TEST(ImportExportTest, ExportImportBinary)
	{
		TestHelper::TemporaryPath initialOutput;
		RunCoverage({ BuildExportTypeString(cov::ExportOptionParser::ExportTypeBinaryValue, initialOutput) }, initialOutput);

		TestHelper::TemporaryPath finalOutput;

		RunCoverage(
		{ { cov::ProgramOptions::InputCoverageValue, initialOutput.GetPath().string() },
		{ BuildExportTypeString(cov::ExportOptionParser::ExportTypeBinaryValue, finalOutput )} },
		finalOutput);	

		auto initialCoverage = ReadCoverageDataFromFile(initialOutput);
		auto finalCoverage = ReadCoverageDataFromFile(finalOutput);

		TestHelper::CoverageDataComparer coverageDataComparer;

		coverageDataComparer.AssertEquals(initialCoverage, finalCoverage);
	}	
}
