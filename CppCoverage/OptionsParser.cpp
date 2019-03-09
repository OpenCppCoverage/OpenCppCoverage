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
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "Tools/Tool.hpp"
#include "CppCoverage/Patterns.hpp"

#include "Options.hpp"
#include "CppCoverageException.hpp"
#include "ProgramOptions.hpp"
#include "OptionsExport.hpp"
#include "ProgramOptionsVariablesMap.hpp"

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
		struct OptionsParserException : std::runtime_error
		{
			OptionsParserException(const std::string& message)
			    : std::runtime_error(message.c_str())
			{
			}
		};

		//---------------------------------------------------------------------
		void CheckPattern(const std::string& name, const std::string& value)
		{
			auto notWindowsPathSeparator = '/';

			if (value.find(notWindowsPathSeparator) != std::string::npos)
			{
				throw OptionsParserException(
				    "Error: Invalid value \""
				    "--" +
				    name + ' ' + value + "\". " +
				    "Please use Windows path separator '\\'.");
			}

			// Do not iterate over path: path{"Folder\\"}.filename() == '.'
			std::vector<std::string> parts;
			boost::split(parts, value, [](char c) { return c == '\\'; });

			for (const auto& part : parts)
			{
				if (part == "." || part == "..")
				{
					throw OptionsParserException(
					    "Error: \""
					    "--" +
					    name + ' ' + value +
					    "\": Cannot contain \".\" (current folder) or \"..\" "
					    "(parent folder).");
				}
			}
		}

		//---------------------------------------------------------------------
		cov::Patterns
		GetPatterns(const ProgramOptionsVariablesMap& variablesMap,
		            const std::string& selected,
		            const std::string& excluded)
		{
			cov::Patterns patterns{false};

			auto selectedPatterns =
			    variablesMap.GetValue<std::vector<std::string>>(selected);
			for (const auto& pattern : selectedPatterns)
			{
				CheckPattern(selected, pattern);
				patterns.AddSelectedPatterns(Tools::LocalToWString(pattern));
			}

			auto excludedPatterns =
			    variablesMap.GetOptionalValue<std::vector<std::string>>(
			        excluded);
			if (excludedPatterns)
			{
				for (const auto& pattern : *excludedPatterns)
				{
					CheckPattern(excluded, pattern);
					patterns.AddExcludedPatterns(
					    Tools::LocalToWString(pattern));
				}
			}

			return patterns;
		}

		//---------------------------------------------------------------------
		boost::optional<cov::StartInfo>
		GetStartInfo(const ProgramOptionsVariablesMap& variablesMap)
		{
			const auto* programToRun =
			    variablesMap.GetOptionalValue<std::string>(
			        ProgramOptions::ProgramToRunOption);

			if (!programToRun)
				return boost::none;

			cov::StartInfo startInfo{*programToRun};

			const auto* arguments =
			    variablesMap.GetOptionalValue<std::vector<std::string>>(
			        ProgramOptions::ProgramToRunArgOption);
			if (arguments)
			{
				for (const auto& arg : *arguments)
					startInfo.AddArgument(Tools::LocalToWString(arg));
			}

			const auto* workingDirectory =
			    variablesMap.GetOptionalValue<std::string>(
			        ProgramOptions::WorkingDirectoryOption);

			if (workingDirectory)
				startInfo.SetWorkingDirectory(*workingDirectory);
			return startInfo;
		}

		//-------------------------------------------------------------------------
		void ParseConfigFile(const ProgramOptions& programOptions,
		                     ProgramOptionsVariablesMap& variablesMap,
		                     const std::string& path)
		{
			std::ifstream ifs(path.c_str());

			if (!ifs)
				throw OptionsParserException("Cannot open config file: " +
				                             path);

			programOptions.FillVariableMap(ifs, variablesMap.GetVariablesMap());
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
		void AddInputCoverages(const ProgramOptionsVariablesMap& variablesMap,
		                       Options& options)
		{
			auto inputCoveragePaths =
			    variablesMap.GetOptionalValue<std::vector<std::string>>(
			        ProgramOptions::InputCoverageValue);

			if (inputCoveragePaths)
			{
				for (const auto& path : *inputCoveragePaths)
				{
					if (!fs::exists(path))
					{
						throw OptionsParserException(
						    "Argument of " +
						    ProgramOptions::InputCoverageValue + " <" + path +
						    "> does not exist.");
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
				return {option, boost::none};

			return {option.substr(0, pos), fs::path{option.substr(pos + 1)}};
		}

		//----------------------------------------------------------------------------
		void AddUnifiedDiff(const ProgramOptionsVariablesMap& variablesMap,
		                    Options& options)
		{
			auto unifiedDiffCollection =
			    variablesMap.GetOptionalValue<std::vector<std::string>>(
			        ProgramOptions::UnifiedDiffOption);

			if (unifiedDiffCollection)
			{
				for (const auto& unifiedDiff : *unifiedDiffCollection)
				{
					fs::path unifiedDiffPath;
					boost::optional<fs::path> rootDiffFolder;

					std::tie(unifiedDiffPath, rootDiffFolder) =
					    ExtractUnifiedDiffOption(unifiedDiff);

					if (!fs::is_regular_file(unifiedDiffPath))
					{
						throw OptionsParserException("Unified diff path " +
						                             unifiedDiffPath.string() +
						                             " does not exist.");
					}
					if (rootDiffFolder && !is_directory(*rootDiffFolder))
					{
						throw OptionsParserException(
						    "Unified diff root folder " +
						    rootDiffFolder->string() + " does not exist.");
					}

					options.AddUnifiedDiffSettings(
					    UnifiedDiffSettings{unifiedDiffPath, rootDiffFolder});
				}
			}
		}

		//----------------------------------------------------------------------------
		void
		AddExcludedLineRegexes(const ProgramOptionsVariablesMap& variablesMap,
		                       Options& options)
		{
			auto excludedLineRegexes =
			    variablesMap.GetOptionalValue<std::vector<std::string>>(
			        ProgramOptions::ExcludedLineRegexOption);
			if (excludedLineRegexes)
			{
				for (const auto& excludedLineRegex : *excludedLineRegexes)
					options.AddExcludedLineRegex(
					    Tools::LocalToWString(excludedLineRegex));
			}
		}

		//---------------------------------------------------------------------
		SubstitutePdbSourcePath
		CreateSubstitutePdbSourcePath(const std::string& paths)
		{
			auto pos = paths.find(OptionsParser::PathSeparator);
			const auto error = "Invalid value for " +
			                   ProgramOptions::SubstitutePdbSourcePathOption +
			                   ". ";
			if (pos == std::string::npos)
			{
				throw OptionsParserException(error + "Cannot find " +
				                             OptionsParser::PathSeparator +
				                             '.');
			}

			auto pdbPath = paths.substr(0, pos);
			if (pdbPath.find('/') != std::string::npos)
			{
				throw OptionsParserException(
				    error + "Path \"" + pdbPath +
				    "\" contains '/' which is not the Windows "
				    "path separator. "
				    "Please use '\\' instead.");
			}
			auto localPath = paths.substr(pos + 1);
			if (!fs::exists(localPath))
			{
				throw OptionsParserException(error + "Path \"" + localPath +
				                             "\" does not exist.");
			}

			return SubstitutePdbSourcePath{pdbPath, localPath};
		}

		//---------------------------------------------------------------------
		void AddSubstitutePdbSourcePaths(
		    const ProgramOptionsVariablesMap& variablesMap, Options& options)
		{
			auto substitutePdbSourcePaths =
			    variablesMap.GetOptionalValue<std::vector<std::string>>(
			        ProgramOptions::SubstitutePdbSourcePathOption);

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
			size_t estimatedSize = 0;
			for (int i = 0; i < argc; ++i)
			{
				auto argument = argv[i];
				if (argument)
					estimatedSize += strlen(argument);
			}

			estimatedSize += argc - 1; // separators for arguments.
			if (estimatedSize >=
			    static_cast<size_t>(OptionsParser::DosCommandLineMaxSize * 3 /
			                        4))
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
			THROW("OptionalWarningManager is null");
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

		programOptions_ = std::make_unique<ProgramOptions>();
	}

	//-------------------------------------------------------------------------
	OptionsParser::~OptionsParser()
	{
	}

	//-------------------------------------------------------------------------
	boost::optional<Options>
	OptionsParser::Parse(int argc,
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
	void OptionsParser::ShowExplanation(std::wostream* emptyOptionsExplanation,
	                                    const char* message) const
	{
		if (emptyOptionsExplanation)
		{
			*emptyOptionsExplanation << message << std::endl;
			*emptyOptionsExplanation << *programOptions_ << std::endl;
		}
	}

	//-------------------------------------------------------------------------
	boost::optional<Options> OptionsParser::Parse(int argc,
	                                              const char** argv) const
	{
		ProgramOptionsVariablesMap variablesMap;

		if (optionalWarningManager_)
			CheckArgumentsSize(argc, argv, *optionalWarningManager_);
		programOptions_->FillVariableMap(
		    argc, argv, variablesMap.GetVariablesMap());
		const auto* configFile = variablesMap.GetOptionalValue<std::string>(
		    ProgramOptions::ConfigFileOption);

		if (configFile)
			ParseConfigFile(*programOptions_, variablesMap, *configFile);

		if (variablesMap.IsOptionSelected(ProgramOptions::HelpOption))
			return boost::none;

		auto modulePatterns =
		    GetPatterns(variablesMap,
		                ProgramOptions::SelectedModulesOption,
		                ProgramOptions::ExcludedModulesOption);
		auto sourcePatterns =
		    GetPatterns(variablesMap,
		                ProgramOptions::SelectedSourcesOption,
		                ProgramOptions::ExcludedSourcesOption);

		auto optionalStartInfo = GetStartInfo(variablesMap);
		Options options{
		    modulePatterns, sourcePatterns, optionalStartInfo.get_ptr()};

		bool isVerbose =
		    variablesMap.IsOptionSelected(ProgramOptions::VerboseOption);
		bool isQuiet =
		    variablesMap.IsOptionSelected(ProgramOptions::QuietOption);

		if (isVerbose && isQuiet)
			throw OptionsParserException("--" + ProgramOptions::VerboseOption +
			                             " and --" +
			                             ProgramOptions::QuietOption +
			                             " cannot be used at the same time.");

		if (isVerbose)
			options.SetLogLevel(LogLevel::Verbose);
		if (isQuiet)
			options.SetLogLevel(LogLevel::Quiet);

		if (variablesMap.IsOptionSelected(ProgramOptions::CoverChildrenOption))
			options.EnableCoverChildrenMode();
		if (variablesMap.IsOptionSelected(ProgramOptions::PluginOption))
			options.EnablePlugingMode();
		if (variablesMap.IsOptionSelected(
		        ProgramOptions::NoAggregateByFileOption))
			options.DisableAggregateByFileMode();
		if (variablesMap.IsOptionSelected(
		        ProgramOptions::ContinueAfterCppExceptionOption))
			options.EnableContinueAfterCppExceptionMode();
		if (variablesMap.IsOptionSelected(ProgramOptions::OptimizedBuildOption))
			options.EnableOptimizedBuildSupport();
		if (variablesMap.IsOptionSelected(ProgramOptions::StopOnAssertOption))
			options.EnableStopOnAssertMode();

		AddExporTypes(variablesMap, options);
		AddInputCoverages(variablesMap, options);
		AddUnifiedDiff(variablesMap, options);
		AddExcludedLineRegexes(variablesMap, options);
		AddSubstitutePdbSourcePaths(variablesMap, options);

		if (!options.GetStartInfo() && options.GetInputCoveragePaths().empty())
			throw OptionsParserException(
			    "You must specify a program to execute or use --" +
			    ProgramOptions::InputCoverageValue);

		return options;
	}

	//----------------------------------------------------------------------------
	void
	OptionsParser::AddExporTypes(const ProgramOptionsVariablesMap& variablesMap,
	                             Options& options) const
	{
		auto exportTypeStrCollection =
		    variablesMap.GetValue<std::vector<std::string>>(
		        ProgramOptions::ExportTypeOption);

		for (const auto& exportTypeStr : exportTypeStrCollection)
		{
			auto optionExport = CreateExport(exportTypeStr);

			options.AddExport(optionExport);
		}
	}

	//-------------------------------------------------------------------------
	OptionsExport
	OptionsParser::CreateExport(const std::string& exportStr) const
	{
		auto exportString = ParseExportString(exportStr);
		auto it = exportTypes_.find(exportString.exportType);

		if (it == exportTypes_.end())
			throw OptionsParserException(exportString.exportType +
			                             " is not a valid export type.");

		OptionsExportType type = it->second;
		auto exportOutputPath = exportString.exportOutputPath;

		if (!exportOutputPath.empty())
			return OptionsExport{type, exportOutputPath};

		return OptionsExport{type};
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
