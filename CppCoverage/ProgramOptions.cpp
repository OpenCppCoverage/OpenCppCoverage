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
#include "ProgramOptions.hpp"

#include <boost/make_shared.hpp>

#include "Tools/Tool.hpp"

namespace po = boost::program_options;

namespace CppCoverage
{
	namespace
	{
		using T_Strings = std::vector<std::string>;

		//-------------------------------------------------------------------------
		void FillGenericOptions(po::options_description& options)
		{			
			options.add_options()
				((ProgramOptions::VerboseOption + "," + ProgramOptions::VerboseShortOption).c_str(), "Show verbose log.")
				((ProgramOptions::HelpOption + "," + ProgramOptions::HelpShortOption).c_str(), "Show help message.")
				(ProgramOptions::ConfigFileOption.c_str(), po::value<std::string>(), "Filename of a configuration file.");
		}

		//-------------------------------------------------------------------------
		void FillConfigurationOptions(po::options_description& options)
		{
			const std::string all = "*";
			
			options.add_options()
				(ProgramOptions::SelectedModulesOption.c_str(),
				po::value<T_Strings>()->default_value(T_Strings{ { all } }, all)->composing(),
				"The pattern that module's paths should match. Can have multiple occurrence.")
				(ProgramOptions::ExcludedModulesOption.c_str(),
				po::value<T_Strings>()->composing(),
				"The pattern that module's paths should NOT match. Can have multiple occurrence.")
				(ProgramOptions::SelectedSourcesOption.c_str(),
				po::value<T_Strings>()->default_value(T_Strings{ { all } }, all)->composing(),
				"The pattern that source's paths should match. Can have multiple occurrence.")
				(ProgramOptions::ExcludedSourcesOption.c_str(),
				po::value<T_Strings>()->composing(),
				"The pattern that source's paths should NOT match. Can have multiple occurrence.")
				(ProgramOptions::WorkingDirectoryOption.c_str(), po::value<std::string>(), "The program working directory.")
				(ProgramOptions::OutputDirectoryOption.c_str(), po::value<std::string>(), "The coverage report directory.");
		}

		//-------------------------------------------------------------------------
		void FillHiddenOptions(po::options_description& options)
		{						
			options.add_options()
				(ProgramOptions::ProgramToRunOption.c_str(), po::value<std::string>())
				(ProgramOptions::ProgramToRunArgOption.c_str(), po::value<T_Strings>());
		}
	}

	//-------------------------------------------------------------------------
	const std::string ProgramOptions::SelectedModulesOption = "modules";
	const std::string ProgramOptions::ExcludedModulesOption = "excluded_modules";
	const std::string ProgramOptions::SelectedSourcesOption = "sources";
	const std::string ProgramOptions::ExcludedSourcesOption = "excluded_sources";
	const std::string ProgramOptions::VerboseOption = "verbose";
	const std::string ProgramOptions::VerboseShortOption = "v";
	const std::string ProgramOptions::HelpOption = "help";
	const std::string ProgramOptions::HelpShortOption = "h";
	const std::string ProgramOptions::ConfigFileOption = "config_file";	
	const std::string ProgramOptions::WorkingDirectoryOption = "working_dir";
	const std::string ProgramOptions::OutputDirectoryOption = "output";
	const std::string ProgramOptions::ProgramToRunOption = "programToRun";
	const std::string ProgramOptions::ProgramToRunArgOption = "programToRunArg";
	
	//-------------------------------------------------------------------------
	ProgramOptions::ProgramOptions()
		: visibleOptions_{ "Usage: [options] -- program_to_run optional_arguments" }
		, configurationOptions_{"Command line and configuration file"}
		, hiddenOptions_{"Hidden"}
		, genericOptions_{"Command line only"}
	{		
		FillGenericOptions(genericOptions_);
		FillConfigurationOptions(configurationOptions_);
		FillHiddenOptions(hiddenOptions_);

		positionalOptions_.add(ProgramToRunOption.c_str(), 1);
		positionalOptions_.add(ProgramToRunArgOption.c_str(), -1);
		
		commandLineOptions_.add(genericOptions_).add(configurationOptions_).add(hiddenOptions_);
		configFileOptions_.add(configurationOptions_).add(hiddenOptions_);
		visibleOptions_.add(genericOptions_).add(configurationOptions_);
	}

	//-------------------------------------------------------------------------
	void ProgramOptions::FillVariableMap(
		int argc,
		const char** argv,
		po::variables_map& variables) const
	{					
		po::store(po::command_line_parser(argc, argv)
			.options(commandLineOptions_)
			.positional(positionalOptions_)
			.run(), variables);
		po::notify(variables);		
	}

	//-------------------------------------------------------------------------
	void ProgramOptions::FillVariableMap(
		std::istream& istr,
		boost::program_options::variables_map& variables) const
	{
		po::store(po::parse_config_file(istr, configFileOptions_), variables);
		po::notify(variables);
	}

	//-------------------------------------------------------------------------
	std::wostream& operator<<(
		std::wostream& ostr,
		const ProgramOptions& programOptions)
	{
		std::ostringstream output;

		output << "OpenCppCoverage Version: " << OPENCPPCOVERAGE_VERSION << std::endl;
		output << std::endl;
		programOptions.visibleOptions_.print(output);

		return ostr << Tools::ToWString(output.str());
	}
}
