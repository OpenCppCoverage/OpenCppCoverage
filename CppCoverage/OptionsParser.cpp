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
#include "ProgramOptions.hpp"

namespace po = boost::program_options;
namespace cov = CppCoverage;

namespace CppCoverage
{
	namespace
	{		
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
		struct OptionsParserException: std::runtime_error
		{
			OptionsParserException(const std::string& message)
				: std::runtime_error(message.c_str())
			{
			}			
		};

		//---------------------------------------------------------------------
		cov::StartInfo GetStartInfo(const po::variables_map& variables)
		{
			const auto* programToRun = GetOptionalValue<std::string>(variables, ProgramOptions::ProgramToRunOption);

			if (!programToRun)
				throw OptionsParserException("No program specified.");

			cov::StartInfo startInfo{ *programToRun };

			const auto* arguments = GetOptionalValue<std::vector<std::string>>(variables, ProgramOptions::ProgramToRunArgOption);
			if (arguments)
			{
				for (const auto& arg : *arguments)
					startInfo.AddArguments(Tools::ToWString(arg));
			}

			const auto* workingDirectory = GetOptionalValue<std::string>(variables, 
				ProgramOptions::WorkingDirectoryOption);
			
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

		//-------------------------------------------------------------------------
		void ParseConfigFile(
			const ProgramOptions& programOptions, 
			po::variables_map& variables, 
			const std::string& path)
		{
			std::ifstream ifs(path.c_str());

			if (!ifs)
				throw OptionsParserException("Cannot open config file: " + path);

			programOptions.FillVariableMap(ifs, variables);
		}		
	}
		
	//-------------------------------------------------------------------------
	OptionsParser::OptionsParser()
	{
		exportTypes_.emplace(ProgramOptions::ExportTypeHtmlValue, OptionsExportType::Html);
		exportTypes_.emplace(ProgramOptions::ExportTypeCoberturaValue, OptionsExportType::Cobertura);

		std::vector<std::string> optionsExportTypes;

		for (const auto& pair : exportTypes_)
			optionsExportTypes.push_back(pair.first);

		programOptions_.reset(new ProgramOptions(optionsExportTypes));
	}

	//-------------------------------------------------------------------------
	OptionsParser::~OptionsParser()
	{
	}

	//-------------------------------------------------------------------------
	boost::optional<Options> OptionsParser::Parse(
			int argc, 
			const char** argv, 
			std::wostream* emptyOptionsExplanation) const
	{				
		try
		{
			auto options = Parse(argc, argv);

			if (!options && emptyOptionsExplanation)
				*emptyOptionsExplanation << *programOptions_;

			return options;
		}
		catch (const boost::program_options::unknown_option& unknownOption)
		{
			ShowExplanation(emptyOptionsExplanation, unknownOption.what());
		}
		catch (const OptionsParserException& e)
		{
			ShowExplanation(emptyOptionsExplanation, e.what());
		}
		
		return boost::none;
	}

	//-------------------------------------------------------------------------
	void OptionsParser::ShowExplanation(
		std::wostream* emptyOptionsExplanation, 
		const char* message) const
	{
		if (emptyOptionsExplanation)
		{
			*emptyOptionsExplanation << message << std::endl;
			*emptyOptionsExplanation << *programOptions_ << std::endl;
		}
	}
		
	//-------------------------------------------------------------------------
	boost::optional<Options> OptionsParser::Parse(int argc, const char** argv) const
	{
		po::variables_map variables;

		programOptions_->FillVariableMap(argc, argv, variables);
		const auto* configFile = GetOptionalValue<std::string>(variables, ProgramOptions::ConfigFileOption);

		if (configFile)
			ParseConfigFile(*programOptions_, variables, *configFile);

		if (IsOptionSelected(variables, ProgramOptions::HelpOption))
			return boost::none;

		auto modulePatterns = GetPatterns(variables, ProgramOptions::SelectedModulesOption, ProgramOptions::ExcludedModulesOption);
		auto sourcePatterns = GetPatterns(variables, ProgramOptions::SelectedSourcesOption, ProgramOptions::ExcludedSourcesOption);

		auto startInfo = GetStartInfo(variables);
		Options options{ startInfo, modulePatterns, sourcePatterns };
		
		if (IsOptionSelected(variables, ProgramOptions::VerboseOption))
			options.SetVerboseModeSelected();

		AddExporTypes(variables, *programOptions_, options);
		const auto* outputDirectoryOption = GetOptionalValue<std::string>(variables, ProgramOptions::OutputDirectoryOption);

		if (outputDirectoryOption)
			options.SetOutputDirectoryOption(*outputDirectoryOption);

		return options;
	}

	//----------------------------------------------------------------------------
	void OptionsParser::AddExporTypes(
		const po::variables_map& variables,
		const ProgramOptions& programOptions,
		Options& options) const
	{
		auto exportTypeStrCollection = GetValue<std::vector<std::string>>(variables, ProgramOptions::ExportTypeOption);

		for (const auto& exportTypeStr : exportTypeStrCollection)
		{
			auto exportType = GetExportType(exportTypeStr);

			options.AddExportType(exportType);
		}
	}
	//-------------------------------------------------------------------------
	OptionsExportType OptionsParser::GetExportType(const std::string& exportType) const
	{
		auto it = exportTypes_.find(exportType);

		if (it == exportTypes_.end())
			throw OptionsParserException(exportType + " is not a valid export type.");
		return it->second;
	}
}
