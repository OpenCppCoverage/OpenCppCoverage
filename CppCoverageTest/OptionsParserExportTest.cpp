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

#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/ProgramOptions.hpp"
#include "CppCoverage/OptionsExport.hpp"

#include "CppCoverageTest/TestTools.hpp"

#include "tools/TemporaryPath.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		bool operator==(const cov::OptionsExport& export1, const cov::OptionsExport& export2)
		{
			return export1.GetType() == export2.GetType()
				&& export1.GetOutputPath() == export2.GetOutputPath();
		}

		//-------------------------------------------------------------------------
		void TestExportTypes(
			const std::vector<std::string>& exportTypesStr,
			const std::vector<cov::OptionsExport>& expectedValue)
		{
			cov::OptionsParser parser;
			std::vector<std::string> arguments;

			for (const auto& exportTypeStr : exportTypesStr)
			{
				arguments.push_back(TestTools::OptionPrefix + cov::ProgramOptions::ExportTypeOption);
				arguments.push_back(exportTypeStr);
			}

			auto options = TestTools::Parse(parser, arguments);

			ASSERT_TRUE(options);
			const auto& exports = options->GetExports();

			ASSERT_EQ(expectedValue.size(), exports.size());

			for (size_t i = 0; i < exports.size(); ++i)
				ASSERT_TRUE(expectedValue[i] == exports[i]);
		}
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesDefault)
	{
		TestExportTypes({}, { cov::OptionsExport{ cov::OptionsExportType::Html } });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesHtml)
	{
		TestExportTypes(
		{ cov::ProgramOptions::ExportTypeHtmlValue },
		{ cov::OptionsExport{ cov::OptionsExportType::Html } });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesCoberturaValue)
	{
		TestExportTypes(
		{ cov::ProgramOptions::ExportTypeCoberturaValue }, 
		{ cov::OptionsExport{ cov::OptionsExportType::Cobertura } });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesBoth)
	{
		TestExportTypes(
		{ cov::ProgramOptions::ExportTypeHtmlValue, cov::ProgramOptions::ExportTypeCoberturaValue },
		{ cov::OptionsExport{ cov::OptionsExportType::Html }, cov::OptionsExport{ cov::OptionsExportType::Cobertura } });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, OutputPath)
	{
		const std::string path = "path";
		TestExportTypes(
		{ cov::ProgramOptions::ExportTypeHtmlValue + cov::OptionsParser::ExportSeparator + path},
		{ cov::OptionsExport{ cov::OptionsExportType::Html, path } });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, InvalidOutputPath)
	{	
		cov::OptionsParser parser;
		Tools::TemporaryPath temporaryPath{ true };

		std::string exportStr = cov::ProgramOptions::ExportTypeHtmlValue + 
			cov::OptionsParser::ExportSeparator + temporaryPath.GetPath().string();
		auto options = TestTools::Parse(parser, { TestTools::OptionPrefix + cov::ProgramOptions::ExportTypeOption, 
								exportStr });
		ASSERT_FALSE(options);		
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, InvalidExportTypes)
	{
		cov::OptionsParser parser;

		auto options = TestTools::Parse(parser, { TestTools::OptionPrefix + cov::ProgramOptions::ExportTypeOption, "Invalid" });
		ASSERT_FALSE(options);
	}
}