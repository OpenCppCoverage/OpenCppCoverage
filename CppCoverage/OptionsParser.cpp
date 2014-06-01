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
#include "OptionsParser.hpp"

#include <string>
#include <vector>
#include <sstream>

#include "Tools/Tool.hpp"
#include "CppCoverage/Patterns.hpp"

#include "Options.hpp"
#include "CppCoverageException.hpp"

namespace po = boost::program_options;
namespace cov = CppCoverage;

namespace CppCoverage
{
	namespace
	{
		//---------------------------------------------------------------------
		// Hidden options
		const std::string ProgramToRunOption = "programToRun";
		const std::string ProgramToRunArgOption = "programToRunArg";
		
		//---------------------------------------------------------------------
		template<typename T>
		const T* GetOptionalValue(
			const po::variables_map& variables, 
			const std::string& optionName)
		{
			auto it = variables.find(optionName);

			if (it == variables.end())
				return nullptr;
			const auto& variable = it->second;

			return &variable.as<T>();
		}

		//---------------------------------------------------------------------
		template<typename T>
		const T& GetValue(
			const po::variables_map& variables,
			const std::string& optionName)
		{
			auto* optionalValue = GetOptionalValue<T>(variables, optionName);
			
			if (!optionalValue)
				THROW(L"Cannot find option:" + Tools::ToWString(optionName));
			
			return *optionalValue;
		}

		//---------------------------------------------------------------------
		cov::Patterns GetPatterns(
			const po::variables_map& variables, 
			const std::string& selected, 
			const std::string& excluded)
		{
			cov::Patterns patterns{ false };

			auto selectedPatterns = GetValue<std::vector<std::string>>(variables, selected);
			for (const auto& pattern : selectedPatterns)
				patterns.AddSelectedPatterns(Tools::ToWString(pattern));

			auto excludedPatterns = GetOptionalValue<std::vector<std::string>>(variables, excluded);
			if (excludedPatterns)
			{
				for (const auto& pattern : *excludedPatterns)
					patterns.AddExcludedPatterns(Tools::ToWString(pattern));
			}

			return patterns;
		}

		//---------------------------------------------------------------------
		cov::StartInfo GetStartInfo(const po::variables_map& variables)
		{
			const auto* programToRun = GetOptionalValue<std::string>(variables, ProgramToRunOption);

			if (!programToRun)
				THROW("No program specified.");

			cov::StartInfo startInfo{ *programToRun };

			const auto* arguments = GetOptionalValue<std::vector<std::string>>(variables, ProgramToRunArgOption);
			if (arguments)
			{
				for (const auto& arg : *arguments)
					startInfo.AddArguments(Tools::ToWString(arg));
			}

			const auto* workingDirectory = GetOptionalValue<std::string>(variables, 
				OptionsParser::WorkingDirectoryOption);
			
			if (workingDirectory)
				startInfo.SetWorkingDirectory(*workingDirectory);
			return startInfo;
		}

		//---------------------------------------------------------------------
		bool IsOptionSelected(
			const po::variables_map& variables,
			const std::string& optionName)
		{
			return variables.find(optionName) != variables.end();
		}
	}

	//-------------------------------------------------------------------------
	const std::string OptionsParser::SelectedModulesOption  = "modules";
	const std::string OptionsParser::ExcludedModulesOption = "excluded_modules";
	const std::string OptionsParser::SelectedSourcesOption = "sources";
	const std::string OptionsParser::ExcludedSourcesOption = "excluded_sources";
	const std::string OptionsParser::VerboseOption = "verbose";
	const std::string OptionsParser::VerboseShortOption = "v";
	const std::string OptionsParser::HelpOption = "help";
	const std::string OptionsParser::HelpShortOption = "h";
	const std::string OptionsParser::WorkingDirectoryOption = "working_dir";
	const std::string OptionsParser::OutputDirectoryOption = "output";
	
	using T_Strings = std::vector<std::string>;

	//-------------------------------------------------------------------------
	OptionsParser::OptionsParser()
		: description_("Usage: [options] -- program_to_run optional_arguments")
	{		
		const std::string all = "*";
				
		description_.add_options()
			(SelectedModulesOption.c_str(),
			po::value<T_Strings>()->default_value(T_Strings{ { all } }, all),
			"The pattern that module's paths should match.")
			(ExcludedModulesOption.c_str(),
			po::value<T_Strings>(),
			"The pattern that module's paths should NOT match.")
			(SelectedSourcesOption.c_str(),
			po::value<T_Strings>()->default_value(T_Strings{ { all } }, all),
			"The pattern that source's paths should match.")
			(ExcludedSourcesOption.c_str(),
			po::value<T_Strings>(),
			"The pattern that source's paths should NOT match.")
			(WorkingDirectoryOption.c_str(), po::value<std::string>(), "The program working directory.")
			(OutputDirectoryOption.c_str(), po::value<std::string>(), "The coverage report directory.")
			((VerboseOption + "," + VerboseShortOption).c_str(), "Show verbose log.")
			((HelpOption + "," + HelpShortOption).c_str(), "Show help message.");
	}
	
	//-------------------------------------------------------------------------
	void OptionsParser::FillVariableMap(
		int argc, 
		const char** argv, 
		po::variables_map& variables) const
	{
		po::options_description hiddenDescription("Hidden");

		hiddenDescription.add_options()
			(ProgramToRunOption.c_str(), po::value<std::string>())
			(ProgramToRunArgOption.c_str(), po::value<T_Strings>());
	
		po::positional_options_description positionalOptions;
		positionalOptions.add(ProgramToRunOption.c_str(), 1);
		positionalOptions.add(ProgramToRunArgOption.c_str(), -1);		
		hiddenDescription.add(description_);
		
		po::store(po::command_line_parser(argc, argv)
			.options(hiddenDescription)
			.positional(positionalOptions)
			.run(), variables);
		po::notify(variables);
	}

	//-------------------------------------------------------------------------
	boost::optional<Options> OptionsParser::Parse(int argc, const char** argv) const
	{						
		po::variables_map variables;
		
		FillVariableMap(argc, argv, variables);

		auto modulePatterns = GetPatterns(variables, SelectedModulesOption, ExcludedModulesOption);
		auto sourcePatterns = GetPatterns(variables, SelectedSourcesOption, ExcludedSourcesOption);

		auto startInfo = GetStartInfo(variables);
		Options options{ startInfo, modulePatterns, sourcePatterns };

		if (IsOptionSelected(variables, HelpOption))
			return boost::optional<Options>();
		if (IsOptionSelected(variables, VerboseOption))
			options.SetVerboseModeSelected();

		const auto* outputDirectoryOption = GetOptionalValue<std::string>(variables, OptionsParser::OutputDirectoryOption);

		if (outputDirectoryOption)
			options.SetOutputDirectoryOption(*outputDirectoryOption);

		return options;
	}

	//-------------------------------------------------------------------------
	std::wostream& operator<<(
		std::wostream& ostr, 
		const OptionsParser& optionsParser)
	{
		std::ostringstream output;

		output << "OpenCppCoverage Version: " << OPENCPPCOVERAGE_VERSION << std::endl;
		output << std::endl;
		optionsParser.description_.print(output);
		
		return ostr << Tools::ToWString(output.str());
	}
}
