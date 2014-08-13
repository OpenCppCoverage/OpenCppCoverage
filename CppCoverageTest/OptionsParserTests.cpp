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
#include "CppCoverage/Options.hpp"
#include "CppCoverage/ProgramOptions.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

#include "Tools/Tool.hpp"
#include "Tools/TemporaryPath.hpp"

namespace po = boost::program_options;
namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace CppCoverageTest
{
	namespace
	{
		const std::string optionPrefix = "--";
		const std::string optionShortPrefix = "-";
		const std::string programToRun = TestCoverageConsole::GetOutputBinaryPath().string();

		//---------------------------------------------------------------------
		boost::optional<cov::Options> Parse(
			const cov::OptionsParser& parser,
			const std::vector<std::string>& arguments,
			bool appendProgramToRun = true)
		{
			std::vector<const char*> argv;

			argv.push_back("programName");
			for (const auto& argument : arguments)
				argv.push_back(argument.c_str());	

			if (appendProgramToRun)
				argv.push_back(programToRun.c_str());
			return parser.Parse(static_cast<int>(argv.size()), &argv[0], nullptr);
		}	

		//-------------------------------------------------------------------------
		void CheckPatternOption(
			const std::string& optionName,
			const std::string& value,
			std::function<std::wstring (const cov::Options&)> getOption)
		{
			cov::OptionsParser parser;
			std::vector<std::string> arguments = { optionPrefix + optionName, value };

			auto options = Parse(parser, arguments);
			auto option = getOption(*options);

			ASSERT_EQ(Tools::ToWString(value), option);
		}
		
		//-------------------------------------------------------------------------	
		boost::optional<cov::Options> MutipleSourceParse(
			const std::vector<std::pair<std::string, std::wstring>>& configArguments,
			const std::vector<std::pair<std::string, std::wstring>>& commandLineArguments)
		{
			cov::OptionsParser parser;
			Tools::TemporaryPath path;
			std::wofstream ofs(path.GetPath().string().c_str());

			for (const auto& argument : configArguments)
				ofs << Tools::ToWString(argument.first) << L"=" << argument.second << std::endl;

			std::vector<std::string> arguments {"--config_file", path.GetPath().string() };

			for (const auto& argumentValue : commandLineArguments)
			{
				arguments.push_back(optionPrefix + argumentValue.first);
				arguments.push_back(Tools::ToString(argumentValue.second));
			}

			return Parse(parser, arguments);
		}

		//-------------------------------------------------------------------------
		void TestExportTypes(
			const std::vector<std::string>& exportTypesStr,
			const std::vector<cov::OptionsExportType>& expectedValue)
		{
			cov::OptionsParser parser;
			std::vector<std::string> arguments;

			for (const auto& exportTypeStr : exportTypesStr)
			{
				arguments.push_back(optionPrefix + cov::ProgramOptions::ExportTypeOption);
				arguments.push_back(exportTypeStr);
			}

			auto options = Parse(parser, arguments);

			ASSERT_TRUE(options);

			const auto& exportTypes = options->GetExportTypes();

			ASSERT_EQ(expectedValue.size(), exportTypes.size());

			for (size_t i = 0; i < expectedValue.size(); ++i)
				ASSERT_EQ(expectedValue[i], exportTypes[i]);
		}
	}
		
	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, Default)
	{
		cov::OptionsParser parser;

		auto options = Parse(parser, {});
		ASSERT_TRUE(options);
		ASSERT_FALSE(options->IsVerboseModeSelected());			
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, Help)
	{
		cov::OptionsParser parser;

		ASSERT_FALSE(Parse(parser, 
		{ optionShortPrefix + cov::ProgramOptions::HelpShortOption }, false));
		ASSERT_FALSE(Parse(parser, 
		{ optionPrefix + cov::ProgramOptions::HelpOption }, false));
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, Verbose)
	{
		cov::OptionsParser parser;

		ASSERT_TRUE(Parse(parser, 
		{ optionShortPrefix + cov::ProgramOptions::VerboseShortOption })->IsVerboseModeSelected());
		ASSERT_TRUE(Parse(parser, 
			{ optionPrefix + cov::ProgramOptions::VerboseOption })->IsVerboseModeSelected());
	}
	
	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, SelectedModulePatterns)
	{
		CheckPatternOption(cov::ProgramOptions::SelectedModulesOption, "module",
			[](const cov::Options& options) { return options.GetModulePatterns().GetSelectedPatterns().front(); }
		);
	}
	
	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ExcludedModulePatterns)
	{
		CheckPatternOption(cov::ProgramOptions::ExcludedModulesOption, "module",
			[](const cov::Options& options) { return options.GetModulePatterns().GetExcludedPatterns().front(); }
		);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, SelectedSourcePatterns)
	{
		CheckPatternOption(cov::ProgramOptions::SelectedSourcesOption, "source",
			[](const cov::Options& options) { return options.GetSourcePatterns().GetSelectedPatterns().front(); }
		);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ExcludedSourcePatterns)
	{
		CheckPatternOption(cov::ProgramOptions::ExcludedSourcesOption, "source",
			[](const cov::Options& options) { return options.GetSourcePatterns().GetExcludedPatterns().front(); }
		);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, WorkingDirectory)
	{
		cov::OptionsParser parser;
		const std::string folder = ".";

		auto options = Parse(parser, 
		{ optionPrefix + cov::ProgramOptions::WorkingDirectoryOption, folder });
		ASSERT_TRUE(options);

		const auto* workingDirectory = options->GetStartInfo().GetWorkingDirectory();

		ASSERT_NE(nullptr, workingDirectory);
		ASSERT_EQ(Tools::ToWString(folder), *workingDirectory);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, OutputDirectory)
	{
		cov::OptionsParser parser;
		const std::string folder = "Output";

		auto options = Parse(parser,
			{ optionPrefix + cov::ProgramOptions::OutputDirectoryOption, folder });
		ASSERT_TRUE(options);

		auto outputDirectoryOption = options->GetOutputDirectoryOption();
		
		ASSERT_TRUE(outputDirectoryOption);
		ASSERT_EQ(folder, outputDirectoryOption->string());		
	}
	
	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, Program)
	{
		cov::OptionsParser parser;		
		const std::string arg1 = "arg1";
		const std::string arg2 = "arg2";
		const std::vector<std::wstring> expectedArgs =
		{ Tools::ToWString(programToRun), Tools::ToWString(arg1), Tools::ToWString(arg2) };

		auto options = Parse(parser, { programToRun, arg1, arg2}, false);
		ASSERT_TRUE(options);

		const auto& startInfo = options->GetStartInfo();		
		ASSERT_EQ(programToRun, startInfo.GetPath().string());
		ASSERT_EQ(expectedArgs, startInfo.GetArguments());
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, UnknownOption)
	{
		cov::OptionsParser parser;

		ASSERT_FALSE(Parse(parser, { "--unknownOption" }));
	}
	
	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ConfigurationFileMultipleValue)
	{
		const std::wstring source1 = L"s1";
		const std::wstring source2 = L"s2";

		auto options = MutipleSourceParse(
		{ { cov::ProgramOptions::SelectedSourcesOption, source1 }, { cov::ProgramOptions::SelectedSourcesOption, source2 } }, 
		{});
		ASSERT_TRUE(options);
		const auto& sourcePatterns = options->GetSourcePatterns().GetSelectedPatterns();
		ASSERT_EQ(2, sourcePatterns.size());
		ASSERT_EQ(source1, sourcePatterns[0]);
		ASSERT_EQ(source2, sourcePatterns[1]);
	}
	
	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ConfigurationFileCmdLineOverride)
	{
		const std::wstring source1 = L"s1";
		const std::wstring source2 = L"s2";

		auto options = MutipleSourceParse(
			{ { cov::ProgramOptions::OutputDirectoryOption, source1 } }, 
			{ { cov::ProgramOptions::OutputDirectoryOption, source2 } });
		ASSERT_TRUE(options);
		auto outputDirectory = options->GetOutputDirectoryOption();
		ASSERT_TRUE(outputDirectory);
		ASSERT_EQ(source2, outputDirectory->wstring());
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ConfigurationFileMerge)
	{
		const std::wstring source1 = L"s1";
		const std::wstring source2 = L"s2";

		auto options = MutipleSourceParse(
			{ {cov::ProgramOptions::SelectedSourcesOption, source1 } }, 
			{ { cov::ProgramOptions::SelectedSourcesOption, source2 } });
		ASSERT_TRUE(options);
		const auto& sourcePatterns = options->GetSourcePatterns().GetSelectedPatterns();
		ASSERT_EQ(2, sourcePatterns.size());
		ASSERT_EQ(source2, sourcePatterns[0]);
		ASSERT_EQ(source1, sourcePatterns[1]);		
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ConfigurationFileInvalidOption)
	{
		auto options = MutipleSourceParse({ { cov::ProgramOptions::ConfigFileOption, L"." } }, {});

		ASSERT_FALSE(options);
	}
	
	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ExportTypesDefault)
	{
		TestExportTypes({}, { cov::OptionsExportType::Html });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ExportTypesHtml)
	{
		TestExportTypes({ cov::ProgramOptions::ExportTypeHtmlValue }, { cov::OptionsExportType::Html });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ExportTypesCoberturaValue)
	{
		TestExportTypes({ cov::ProgramOptions::ExportTypeCoberturaValue }, { cov::OptionsExportType::Cobertura });
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ExportTypesBoth)
	{
		TestExportTypes(
			{ cov::ProgramOptions::ExportTypeHtmlValue, cov::ProgramOptions::ExportTypeCoberturaValue }, 
			{ cov::OptionsExportType::Html, cov::OptionsExportType::Cobertura });		
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, InvalidExportTypes)
	{
		cov::OptionsParser parser;

		auto options = Parse(parser, { optionPrefix + cov::ProgramOptions::ExportTypeOption, "Invalid" });
		ASSERT_FALSE(options);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, OptionOstream)
	{ 
		cov::OptionsParser parser;
		auto options = Parse(parser, { optionPrefix + cov::ProgramOptions::SelectedSourcesOption, "source1" });
		
		std::wostringstream ostr;

		ostr << options;
		ASSERT_LT(0, ostr.str().size());
	}
}