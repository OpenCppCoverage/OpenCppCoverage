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

#include "CppCoverageTest/TestTools.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{

		//-------------------------------------------------------------------------
		void TestExportTypes(
			const std::vector<std::string>& exportTypesStr,
			const std::vector<cov::OptionsExportType>& expectedValue)
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

			const auto& exportTypes = options->GetExportTypes();

			ASSERT_EQ(expectedValue.size(), exportTypes.size());

			for (size_t i = 0; i < expectedValue.size(); ++i)
				ASSERT_EQ(expectedValue[i], exportTypes[i]);
		}
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesDefault)
	{
		TestExportTypes({}, { cov::OptionsExportType::Html });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesHtml)
	{
		TestExportTypes({ cov::ProgramOptions::ExportTypeHtmlValue }, { cov::OptionsExportType::Html });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesCoberturaValue)
	{
		TestExportTypes({ cov::ProgramOptions::ExportTypeCoberturaValue }, { cov::OptionsExportType::Cobertura });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesBoth)
	{
		TestExportTypes(
		{ cov::ProgramOptions::ExportTypeHtmlValue, cov::ProgramOptions::ExportTypeCoberturaValue },
		{ cov::OptionsExportType::Html, cov::OptionsExportType::Cobertura });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, InvalidExportTypes)
	{
		cov::OptionsParser parser;

		auto options = TestTools::Parse(parser, { TestTools::OptionPrefix + cov::ProgramOptions::ExportTypeOption, "Invalid" });
		ASSERT_FALSE(options);
	}
}