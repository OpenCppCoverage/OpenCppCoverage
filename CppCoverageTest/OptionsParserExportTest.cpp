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
#include "CppCoverage/ExportOptionParser.hpp"
#include "CppCoverage/OptionsExport.hpp"

#include "CppCoverageTest/TestTools.hpp"

#include "TestHelper/TemporaryPath.hpp"
#include "Tools/Tool.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		bool operator==(const cov::OptionsExport& export1, const cov::OptionsExport& export2)
		{
			return export1.GetType() == export2.GetType() &&
			       export1.GetParameter() == export2.GetParameter();
		}

		//-------------------------------------------------------------------------
		std::unique_ptr<cov::OptionsParser>
		CreateOptionParser(std::vector<cov::ExportPluginDescription>&&
		                       exportPluginDescription = {})
		{
			std::vector<std::unique_ptr<cov::IOptionParser>> optionParsers;

			optionParsers.push_back(std::make_unique<cov::ExportOptionParser>(
			    std::move(exportPluginDescription)));
			return std::make_unique<cov::OptionsParser>(
			    nullptr, std::move(optionParsers));
		}

		//-------------------------------------------------------------------------
		void TestExportTypes(
			const std::vector<std::string>& exportTypesStr,
			const std::vector<cov::OptionsExport>& expectedValues)
		{
			auto parser = CreateOptionParser();
			std::vector<std::string> arguments;

			for (const auto& exportTypeStr : exportTypesStr)
			{
				arguments.push_back(TestTools::GetOptionPrefix() + cov::ExportOptionParser::ExportTypeOption);
				arguments.push_back(exportTypeStr);
			}

			auto options = TestTools::Parse(*parser, arguments);

			ASSERT_TRUE(static_cast<bool>(options));
			const auto& exports = options->GetExports();

			ASSERT_EQ(expectedValues.size(), exports.size());

			for (size_t i = 0; i < exports.size(); ++i)
				ASSERT_TRUE(expectedValues[i] == exports[i]);
		}

		//-------------------------------------------------------------------------
		void TestExportTypes(
			const std::vector<std::string>& exportTypesStr,
			cov::OptionsExport&& expectedValue)
		{
			std::vector<cov::OptionsExport> expectedValues;

			expectedValues.push_back(std::move(expectedValue));
			TestExportTypes(exportTypesStr, expectedValues);
		}

		//-------------------------------------------------------------------------
		cov::OptionsExport MakeOptionExport(cov::OptionsExportType type)
		{
			return cov::OptionsExport{ type, L"", std::nullopt };
		}
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesDefault)
	{
		TestExportTypes({}, MakeOptionExport(cov::OptionsExportType::Html));
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesHtml)
	{
		TestExportTypes({cov::ExportOptionParser::ExportTypeHtmlValue},
		                MakeOptionExport(cov::OptionsExportType::Html));
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesCoberturaValue)
	{
		TestExportTypes(
		    {cov::ExportOptionParser::ExportTypeCoberturaValue},
		     MakeOptionExport(cov::OptionsExportType::Cobertura));
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportTypesBoth)
	{
		std::vector<cov::OptionsExport> expectedValues;
		expectedValues.push_back(MakeOptionExport(cov::OptionsExportType::Html));
		expectedValues.push_back(MakeOptionExport(cov::OptionsExportType::Cobertura));

		TestExportTypes(
		    {cov::ExportOptionParser::ExportTypeHtmlValue,
		     cov::ExportOptionParser::ExportTypeCoberturaValue},
			expectedValues);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, OutputPath)
	{
		const std::string path = "path";
		TestExportTypes(
		    {cov::ExportOptionParser::ExportTypeHtmlValue +
		     cov::ExportOptionParser::ExportSeparator + path},
			{ cov::OptionsExport{cov::OptionsExportType::Html, L"", Tools::LocalToWString(path)}});
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExistingExportPath)
	{	
		auto parser = CreateOptionParser();
		TestHelper::TemporaryPath temporaryPath{ TestHelper::TemporaryPathOption::CreateAsFile };

		std::string exportStr = cov::ExportOptionParser::ExportTypeHtmlValue + 
			cov::ExportOptionParser::ExportSeparator + temporaryPath.GetPath().string();
		auto options = TestTools::Parse(*parser, { TestTools::GetOptionPrefix() + cov::ExportOptionParser::ExportTypeOption, 
								exportStr });
		ASSERT_NE(nullptr, options.get_ptr());
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, InvalidExportTypes)
	{
		auto parser = CreateOptionParser();

		auto options =
		    TestTools::Parse(*parser,
		                     {TestTools::GetOptionPrefix() +
		                          cov::ExportOptionParser::ExportTypeOption,
		                      "Invalid"});
		ASSERT_FALSE(options);
	}

	namespace
	{
		//-------------------------------------------------------------------------
		void CheckExportPluginParser(
		    const std::wstring& pluginName,
		    const std::wstring& pluginArg,
			std::function<void(const std::wstring&)> checkArgumentFct,
		    std::function<void(const cov::OptionsExport&)> checkOptionExporFct)
		{
			std::vector<cov::ExportPluginDescription> exportPluginDescription;
			exportPluginDescription.push_back(cov::ExportPluginDescription{
				std::wstring{pluginName},
			    L"",
				checkArgumentFct
			});
			auto parser =
			    CreateOptionParser(std::move(exportPluginDescription));

			auto options =
			    TestTools::Parse(*parser,
			                     {TestTools::GetOptionPrefix() +
			                          cov::ExportOptionParser::ExportTypeOption,
			                      Tools::ToLocalString(pluginName) + ':' +
			                          Tools::ToLocalString(pluginArg)});
			if (!options)
				throw std::runtime_error("Null option");

			const auto& e = options->GetExports().at(0);
			checkOptionExporFct(e);
		}
	}
	//-------------------------------------------------------------------------
	TEST(OptionsParserExportTest, ExportPluginDescription)
	{
		const std::wstring pluginArg = L"pluginArg";
		std::wstring argument;

		CheckExportPluginParser(
			L"pluginName",
			pluginArg,
			[&](const auto& arg) { argument = arg;  },
			[&](const cov::OptionsExport& e) {
			ASSERT_EQ(cov::OptionsExportType::Plugin, e.GetType());
			ASSERT_EQ(pluginArg, e.GetParameter());
		});
		ASSERT_EQ(pluginArg, argument);
	}
}