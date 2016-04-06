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

#include "CppCoverageException.hpp"
#include "OptionsParser.hpp"

namespace po = boost::program_options;

namespace CppCoverage
{
	namespace
	{
		using T_Strings = std::vector<std::string>;
		
		//-------------------------------------------------------------------------
		std::string GetExportTypesAsString(const std::vector<std::string>& optionsExportTypes)
		{
			std::string possibleExportType;

			for (const auto& exportType : optionsExportTypes)
			{
				if (!possibleExportType.empty())
					possibleExportType += ", ";
				possibleExportType += exportType;				
			}

			return possibleExportType;
		}

		//---------------------------------------------------------------------
		void FillGenericOptions(po::options_description& options)
		{			
			options.add_options()
				((ProgramOptions::VerboseOption + "," + ProgramOptions::VerboseShortOption).c_str(), "Verbose mode.")
				((ProgramOptions::QuietOption + "," + ProgramOptions::QuietShortOption).c_str(), "Quiet mode.")
				((ProgramOptions::HelpOption + "," + ProgramOptions::HelpShortOption).c_str(), "Show help message.")
				(ProgramOptions::ConfigFileOption.c_str(), po::value<std::string>(), "Filename of a configuration file.");
		}

		//---------------------------------------------------------------------
		std::string GetExportTypeText(const std::vector<std::string>& exportTypes)
		{
			std::string exportTypeText{ "Format: <exportType>:<outputPath>.\n" };
			
			exportTypeText += "<exportType> can be: ";
			exportTypeText += GetExportTypesAsString(exportTypes) + "\n";
			exportTypeText += "<outputPath> (optional) output file or directory for the export.\n";
			exportTypeText += "Example: html:MyFolder\\MySubFolder\n";
			exportTypeText += "This flag can have multiple occurrences.";

			return exportTypeText;
		}

		//---------------------------------------------------------------------
		std::string GetUnifiedDiffHelp()
		{
			return std::string("Format: <unifiedDiffPath>") +
				OptionsParser::UnifiedDiffSeparator +
				"<rootFolder>\n" +
				"<unifiedDiffPath> path of the unified diff file. " +
				"Git users can use git diff output.\n" +
				"<rootFolder> (optional) root folder for paths in the diff file.";
		}

		//---------------------------------------------------------------------
		void FillConfigurationOptions(
			po::options_description& options, 
			const std::vector<std::string>& exportTypes)
		{
			const std::string all = "*";
			
			options.add_options()
				(ProgramOptions::SelectedModulesOption.c_str(),
				po::value<T_Strings>()->default_value({ all }, all)->composing(),
				"The pattern that module's paths should match. Can have multiple occurrences.")
				(ProgramOptions::ExcludedModulesOption.c_str(),
				po::value<T_Strings>()->composing(),
				"The pattern that module's paths should NOT match. Can have multiple occurrences.")
				(ProgramOptions::SelectedSourcesOption.c_str(),
				po::value<T_Strings>()->default_value({ all }, all)->composing(),
				"The pattern that source's paths should match. Can have multiple occurrences.")
				(ProgramOptions::ExcludedSourcesOption.c_str(),
				po::value<T_Strings>()->composing(),
				"The pattern that source's paths should NOT match. Can have multiple occurrences.")
				(ProgramOptions::InputCoverageValue.c_str(), po::value<T_Strings>()->composing(),
				("A output path of " + ProgramOptions::ExportTypeOption + "=" + ProgramOptions::ExportTypeBinaryValue +
				". This coverage data will be merged with the current one. Can have multiple occurrences.").c_str())
				(ProgramOptions::ExportTypeOption.c_str(),
				po::value<T_Strings>()->default_value({ ProgramOptions::ExportTypeHtmlValue }, ProgramOptions::ExportTypeHtmlValue),
				GetExportTypeText(exportTypes).c_str())
				(ProgramOptions::WorkingDirectoryOption.c_str(), po::value<std::string>(), "The program working directory.")
				(ProgramOptions::CoverChildrenOption.c_str(), "Enable code coverage for children processes.")
				(ProgramOptions::NoAggregateByFileOption.c_str(), "Do not aggregate coverage for same file path.")
				(ProgramOptions::UnifiedDiffOption.c_str(),
					po::value<T_Strings>()->composing(), GetUnifiedDiffHelp().c_str())
				(ProgramOptions::ContinueAfterCppExceptionOption.c_str(), "Try to continue after throwing a C++ exception.");
		}

		//-------------------------------------------------------------------------
		void FillHiddenOptions(po::options_description& options)
		{						
			options.add_options()
				((ProgramOptions::PluginOption).c_str(), "Plugin mode.")
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
	const std::string ProgramOptions::QuietOption = "quiet";
	const std::string ProgramOptions::QuietShortOption = "q";
	const std::string ProgramOptions::PluginOption = "plugin";
	const std::string ProgramOptions::HelpOption = "help";
	const std::string ProgramOptions::HelpShortOption = "h";
	const std::string ProgramOptions::ConfigFileOption = "config_file";	
	const std::string ProgramOptions::WorkingDirectoryOption = "working_dir";
	const std::string ProgramOptions::CoverChildrenOption = "cover_children";
	const std::string ProgramOptions::NoAggregateByFileOption = "no_aggregate_by_file";
	const std::string ProgramOptions::ProgramToRunOption = "programToRun";
	const std::string ProgramOptions::ProgramToRunArgOption = "programToRunArg";
	const std::string ProgramOptions::ExportTypeOption = "export_type";
	const std::string ProgramOptions::ExportTypeHtmlValue = "html";
	const std::string ProgramOptions::ExportTypeCoberturaValue = "cobertura";
	const std::string ProgramOptions::ExportTypeBinaryValue = "binary";
	const std::string ProgramOptions::InputCoverageValue = "input_coverage";
	const std::string ProgramOptions::UnifiedDiffOption = "unified_diff";
	const std::string ProgramOptions::ContinueAfterCppExceptionOption = "continue_after_cpp_exception";

	//-------------------------------------------------------------------------
	ProgramOptions::ProgramOptions(const std::vector<std::string>& exportTypes)
		: visibleOptions_{ "Usage: [options] -- program_to_run optional_arguments" }
		, configurationOptions_{"Command line and configuration file"}
		, hiddenOptions_{"Hidden"}
		, genericOptions_{"Command line only"}
	{				
		FillGenericOptions(genericOptions_);
		FillConfigurationOptions(configurationOptions_, exportTypes);
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
