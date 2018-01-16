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
#include <boost/filesystem.hpp>

#include "Tools/Tool.hpp"
#include "CppCoverage/Patterns.hpp"

#include "Options.hpp"
#include "CppCoverageException.hpp"
#include "ProgramOptions.hpp"
#include "OptionsExport.hpp"
#include "Tools/WarningManager.hpp"
#include "Tools/Log.hpp"

namespace po = boost::program_options;
namespace cov = CppCoverage;
namespace fs = boost::filesystem;

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
				THROW(L"Cannot find option:" + Tools::LocalToWString(optionName));
			
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
				patterns.AddSelectedPatterns(Tools::LocalToWString(pattern));

			auto excludedPatterns = GetOptionalValue<std::vector<std::string>>(variables, excluded);
			if (excludedPatterns)
			{
				for (const auto& pattern : *excludedPatterns)
					patterns.AddExcludedPatterns(Tools::LocalToWString(pattern));
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
		boost::optional<cov::StartInfo> GetStartInfo(const po::variables_map& variables)
		{
			const auto* programToRun = GetOptionalValue<std::string>(variables, ProgramOptions::ProgramToRunOption);

			if (!programToRun)
				return boost::none;				

			cov::StartInfo startInfo{ *programToRun };

			const auto* arguments = GetOptionalValue<std::vector<std::string>>(variables, ProgramOptions::ProgramToRunArgOption);
			if (arguments)
			{
				for (const auto& arg : *arguments)
					startInfo.AddArgument(Tools::LocalToWString(arg));
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

		//---------------------------------------------------------------------
		struct ExportString
		{
			std::string exportOutputPath;
			std::string exportType;
		};

		//---------------------------------------------------------------------
		ExportString ParseExportString(const std::string& exportStr)
		{
			ExportString exportString;
			auto pos = exportStr.find(OptionsParser::ExportSeparator);

			if (pos != std::string::npos)
			{
				exportString.exportType = exportStr.substr(0, pos);
				exportString.exportOutputPath = exportStr.substr(pos + 1);
			}
			else
				exportString.exportType = exportStr;

			return exportString;
		}


		//---------------------------------------------------------------------
		void AddInputCoverages(
			const po::variables_map& variables,
			Options& options)
		{
			auto inputCoveragePaths =
				GetOptionalValue<std::vector<std::string>>(variables, ProgramOptions::InputCoverageValue);

			if (inputCoveragePaths)
			{
				for (const auto& path : *inputCoveragePaths)
				{
					if (!fs::exists(path))
					{
						throw OptionsParserException("Argument of " + 
							ProgramOptions::InputCoverageValue + " <" + path + "> does not exist.");
					}

					options.AddInputCoveragePath(path);
				}
			}
		}

		//----------------------------------------------------------------------------
		std::pair<fs::path, boost::optional<fs::path>>
			ExtractUnifiedDiffOption(const std::string& option)
		{
			auto pos = option.find(OptionsParser::PathSeparator);

			if (pos == std::string::npos)
				return{ option, boost::none };

			return{ option.substr(0, pos), fs::path{option.substr(pos + 1)} };
		}

		//----------------------------------------------------------------------------
		void AddUnifiedDiff(const po::variables_map& variables, Options& options)
		{
			auto unifiedDiffCollection = GetOptionalValue<std::vector<std::string>>(
				variables, ProgramOptions::UnifiedDiffOption);

			if (unifiedDiffCollection)
			{
				for (const auto& unifiedDiff : *unifiedDiffCollection)
				{
					fs::path unifiedDiffPath;
					boost::optional<fs::path> rootDiffFolder;

					std::tie(unifiedDiffPath, rootDiffFolder) = ExtractUnifiedDiffOption(unifiedDiff);

					if (!fs::is_regular_file(unifiedDiffPath))
					{
						throw OptionsParserException(
							"Unified diff path " + unifiedDiffPath.string() + " does not exist.");
					}
					if (rootDiffFolder && !is_directory(*rootDiffFolder))
					{
						throw OptionsParserException(
							"Unified diff root folder " + rootDiffFolder->string() + " does not exist.");
					}

					options.AddUnifiedDiffSettings(UnifiedDiffSettings{ unifiedDiffPath, rootDiffFolder });
				}
			}
		}

		//----------------------------------------------------------------------------
		void AddExcludedLineRegexes(const po::variables_map& variables, Options& options)
		{
			auto excludedLineRegexes = GetOptionalValue<std::vector<std::string>>(
				variables, ProgramOptions::ExcludedLineRegexOption);
			if (excludedLineRegexes)
			{
				for (const auto& excludedLineRegex : *excludedLineRegexes)
					options.AddExcludedLineRegex(Tools::LocalToWString(excludedLineRegex));
			}
		}

		//---------------------------------------------------------------------
		SubstitutePdbSourcePath CreateSubstitutePdbSourcePath(const std::string& paths)
		{
			auto pos = paths.find(OptionsParser::PathSeparator);
			const auto error = "Invalid value for " +
			                   ProgramOptions::SubstitutePdbSourcePathOption + ". ";
			if (pos == std::string::npos)
			{
				throw OptionsParserException(error + "Cannot find " +
				                             OptionsParser::PathSeparator +
				                             '.');
			}

			auto pdbPath = paths.substr(0, pos);
			auto localPath = paths.substr(pos + 1);
			if (!fs::exists(localPath))
			{
				throw OptionsParserException(error + "Path \"" + localPath +
				                             "\" does not exist.");
			}

			return SubstitutePdbSourcePath{pdbPath, localPath};
		}

		//---------------------------------------------------------------------
		void AddSubstitutePdbSourcePaths(const po::variables_map& variables,
		                                Options& options)
		{
			auto substitutePdbSourcePaths =
			    GetOptionalValue<std::vector<std::string>>(
			        variables, ProgramOptions::SubstitutePdbSourcePathOption);

			if (substitutePdbSourcePaths)
			{
				for (const auto& paths : *substitutePdbSourcePaths)
				{
					options.AddSubstitutePdbSourcePath(
					    CreateSubstitutePdbSourcePath(paths));
				}
			}
		}
		//---------------------------------------------------------------------------
		void CheckArgumentsSize(int argc,
		                        const char** argv,
		                        Tools::WarningManager& warningManager)
		{
			auto estimatedSize = 0;
			for (int i = 0; i < argc; ++i)
			{
				auto argument = argv[i];
				if (argument)
					estimatedSize += strlen(argument);
			}

			estimatedSize += argc - 1; // separators for arguments.
			if (estimatedSize >= OptionsParser::DosCommandLineMaxSize * 3 / 4)
			{
				auto tooLongCmd = OptionsParser::GetTooLongCommandLineMessage();
				LOG_WARNING << tooLongCmd;
				warningManager.AddWarning(tooLongCmd);
			}
		}
	}

	//-------------------------------------------------------------------------
	const char OptionsParser::ExportSeparator = ':';
	const char OptionsParser::PathSeparator = '?';
	const int OptionsParser::DosCommandLineMaxSize = 8191;

	//-------------------------------------------------------------------------
	OptionsParser::OptionsParser(
	    std::shared_ptr<Tools::WarningManager> warningManager)
	    : OptionsParser()
	{
		optionalWarningManager_ = std::move(warningManager);
		if (!optionalWarningManager_)
			THROW("OptinalWarningManager is null");
	}

	//-------------------------------------------------------------------------
	OptionsParser::OptionsParser()
	{
		exportTypes_.emplace(ProgramOptions::ExportTypeHtmlValue,
		                     OptionsExportType::Html);
		exportTypes_.emplace(ProgramOptions::ExportTypeCoberturaValue,
		                     OptionsExportType::Cobertura);
		exportTypes_.emplace(ProgramOptions::ExportTypeBinaryValue,
		                     OptionsExportType::Binary);

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

		if (optionalWarningManager_)
			CheckArgumentsSize(argc, argv, *optionalWarningManager_);
		programOptions_->FillVariableMap(argc, argv, variables);
		const auto* configFile = GetOptionalValue<std::string>(variables, ProgramOptions::ConfigFileOption);

		if (configFile)
			ParseConfigFile(*programOptions_, variables, *configFile);

		if (IsOptionSelected(variables, ProgramOptions::HelpOption))
			return boost::none;

		auto modulePatterns = GetPatterns(variables, ProgramOptions::SelectedModulesOption, ProgramOptions::ExcludedModulesOption);
		auto sourcePatterns = GetPatterns(variables, ProgramOptions::SelectedSourcesOption, ProgramOptions::ExcludedSourcesOption);

		auto optionalStartInfo = GetStartInfo(variables);
		Options options{ modulePatterns, sourcePatterns, optionalStartInfo.get_ptr() };
		
		bool isVerbose = IsOptionSelected(variables, ProgramOptions::VerboseOption);
		bool isQuiet = IsOptionSelected(variables, ProgramOptions::QuietOption);

		if (isVerbose && isQuiet)
			throw OptionsParserException("--" + ProgramOptions::VerboseOption + " and --" + ProgramOptions::QuietOption + " cannot be used at the same time.");

		if (isVerbose)
			options.SetLogLevel(LogLevel::Verbose);
		if (isQuiet)
			options.SetLogLevel(LogLevel::Quiet);

		if (IsOptionSelected(variables, ProgramOptions::CoverChildrenOption))
			options.EnableCoverChildrenMode();
		if (IsOptionSelected(variables, ProgramOptions::PluginOption))
			options.EnablePlugingMode();
		if (IsOptionSelected(variables, ProgramOptions::NoAggregateByFileOption))
			options.DisableAggregateByFileMode();
		if (IsOptionSelected(variables, ProgramOptions::ContinueAfterCppExceptionOption))
			options.EnableContinueAfterCppExceptionMode();
		if (IsOptionSelected(variables, ProgramOptions::OptimizedBuildOption))
			options.EnableOptimizedBuildSupport();

		AddExporTypes(variables, options);
		AddInputCoverages(variables, options);
		AddUnifiedDiff(variables, options);
		AddExcludedLineRegexes(variables, options);
		AddSubstitutePdbSourcePaths(variables, options);

		if (!options.GetStartInfo() && options.GetInputCoveragePaths().empty())
			throw OptionsParserException("You must specify a program to execute or use --" + ProgramOptions::InputCoverageValue);

		return options;
	}

	//----------------------------------------------------------------------------
	void OptionsParser::AddExporTypes(
		const po::variables_map& variables,
		Options& options) const
	{
		auto exportTypeStrCollection = GetValue<std::vector<std::string>>(variables, ProgramOptions::ExportTypeOption);

		for (const auto& exportTypeStr : exportTypeStrCollection)
		{
			auto optionExport = CreateExport(exportTypeStr);

			options.AddExport(optionExport);
		}
	}
	
	//-------------------------------------------------------------------------
	OptionsExport OptionsParser::CreateExport(const std::string& exportStr) const
	{
		auto exportString = ParseExportString(exportStr);		
		auto it = exportTypes_.find(exportString.exportType);

		if (it == exportTypes_.end())
			throw OptionsParserException(exportString.exportType + " is not a valid export type.");		

		OptionsExportType type = it->second;
		auto exportOutputPath = exportString.exportOutputPath;

		if (!exportOutputPath.empty())
			return OptionsExport{ type, exportOutputPath };

		return OptionsExport{ type };
	}

	//-------------------------------------------------------------------------
	std::wstring OptionsParser::GetTooLongCommandLineMessage()
	{
		return L"You have a very long command line. It might be truncated "
		       "as DOS maximum command line size is " +
		       std::to_wstring(DosCommandLineMaxSize) +
		       L". Please consider using --" +
		       Tools::LocalToWString(ProgramOptions::ConfigFileOption) +
		       L" instead.";
	}
}
