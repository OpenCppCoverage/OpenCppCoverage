#include "stdafx.h"

#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/Options.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

#include "tools/Tool.hpp"

namespace po = boost::program_options;
namespace cov = CppCoverage;

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
			return parser.Parse(argv.size(), &argv[0]);
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
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, Print)
	{
		cov::OptionsParser parser;
		std::wostringstream ostr;

		ASSERT_NO_THROW(ostr << parser);
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
		{ optionShortPrefix + cov::OptionsParser::HelpShortOption }));
		ASSERT_FALSE(Parse(parser, 
		{ optionPrefix + cov::OptionsParser::HelpOption }));
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, Verbose)
	{
		cov::OptionsParser parser;

		ASSERT_TRUE(Parse(parser, 
		{ optionShortPrefix + cov::OptionsParser::VerboseShortOption })->IsVerboseModeSelected());
		ASSERT_TRUE(Parse(parser, 
			{ optionPrefix + cov::OptionsParser::VerboseOption })->IsVerboseModeSelected());
	}
	
	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, SelectedModulePatterns)
	{
		CheckPatternOption(cov::OptionsParser::SelectedModulesOption, "module",
			[](const cov::Options& options) { return options.GetModulePatterns().GetSelectedPatterns().front(); }
		);
	}
	
	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ExcludedModulePatterns)
	{
		CheckPatternOption(cov::OptionsParser::ExcludedModulesOption, "module",
			[](const cov::Options& options) { return options.GetModulePatterns().GetExcludedPatterns().front(); }
		);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, SelectedSourcePatterns)
	{
		CheckPatternOption(cov::OptionsParser::SelectedSourcesOption, "source",
			[](const cov::Options& options) { return options.GetSourcePatterns().GetSelectedPatterns().front(); }
		);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, ExcludedSourcePatterns)
	{
		CheckPatternOption(cov::OptionsParser::ExcludedSourcesOption, "source",
			[](const cov::Options& options) { return options.GetSourcePatterns().GetExcludedPatterns().front(); }
		);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserTest, WorkingDirectory)
	{
		cov::OptionsParser parser;
		const std::string folder = ".";

		auto options = Parse(parser, 
		{ optionPrefix + cov::OptionsParser::WorkingDirectoryOption, folder });
		ASSERT_TRUE(options);

		const auto* workingDirectory = options->GetStartInfo().GetWorkingDirectory();

		ASSERT_NE(nullptr, workingDirectory);
		ASSERT_EQ(Tools::ToWString(folder), *workingDirectory);
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

		ASSERT_THROW(Parse(parser, { "--unknownOption" }), po::unknown_option);
	}
}