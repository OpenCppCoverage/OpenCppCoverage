// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2019 OpenCppCoverage
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
#include "ExportOptionParser.hpp"
#include <boost/program_options/options_description.hpp>

#include "ProgramOptionsVariablesMap.hpp"
#include "Options.hpp"
#include "OptionsParserException.hpp"

namespace CppCoverage
{
	namespace
	{
		//---------------------------------------------------------------------
		struct ExportData
		{
			std::string argument;
			std::string exportType;
		};

		//---------------------------------------------------------------------
		ExportData ParseExportData(const std::string& exportStr)
		{
			ExportData exportData;
			auto pos = exportStr.find(ExportOptionParser::ExportSeparator);

			if (pos != std::string::npos)
			{
				exportData.exportType = exportStr.substr(0, pos);
				exportData.argument = exportStr.substr(pos + 1);
			}
			else
				exportData.exportType = exportStr;

			return exportData;
		}

		//-------------------------------------------------------------------------
		boost::optional<OptionsExport> CreateExport(
		    const std::map<std::string, OptionsExportType>& exportTypes,
		    const ExportData& exportData)
		{
			auto it = exportTypes.find(exportData.exportType);

			if (it != exportTypes.end())
			{
				OptionsExportType type = it->second;
				auto exportOutputPath = exportData.argument;

				if (!exportOutputPath.empty())
					return OptionsExport{type, exportOutputPath};
				return OptionsExport{type};
			}

			return boost::none;
		}

		//---------------------------------------------------------------------
		std::string GetExportTypeText()
		{
			return "Format: <exportType>:<outputPath>.\n"
			       "<exportType> can be: " +
			       ExportOptionParser::ExportTypeHtmlValue + ", " +
			       ExportOptionParser::ExportTypeCoberturaValue + " or " +
			       ExportOptionParser::ExportTypeBinaryValue +
			       "\n<outputPath> (optional) export output path.\n"
			       "Must be a folder for " +
			       ExportOptionParser::ExportTypeHtmlValue +
			       " and a file for " +
			       ExportOptionParser::ExportTypeCoberturaValue + " or " +
			       ExportOptionParser::ExportTypeBinaryValue +
			       ".\nExample: html:MyFolder\\MySubFolder\n"
			       "This flag can have multiple occurrences.";
		}
	}

	const char ExportOptionParser::ExportSeparator = ':';
	const std::string ExportOptionParser::ExportTypeOption = "export_type";
	const std::string ExportOptionParser::ExportTypeHtmlValue = "html";
	const std::string ExportOptionParser::ExportTypeCoberturaValue =
	    "cobertura";
	const std::string ExportOptionParser::ExportTypeBinaryValue = "binary";

	//-------------------------------------------------------------------------
	ExportOptionParser::ExportOptionParser()
	{
		exportTypes_.emplace(ExportOptionParser::ExportTypeHtmlValue,
		                     OptionsExportType::Html);
		exportTypes_.emplace(ExportOptionParser::ExportTypeCoberturaValue,
		                     OptionsExportType::Cobertura);
		exportTypes_.emplace(ExportOptionParser::ExportTypeBinaryValue,
		                     OptionsExportType::Binary);
	}

	//----------------------------------------------------------------------------
	void ExportOptionParser::ParseOption(
	    const ProgramOptionsVariablesMap& variablesMap, Options& options)
	{
		auto exportTypeStrCollection =
		    variablesMap.GetValue<std::vector<std::string>>(
		        ExportOptionParser::ExportTypeOption);

		for (const auto& exportTypeStr : exportTypeStrCollection)
		{
			auto exportData = ParseExportData(exportTypeStr);
			auto optionExport = CreateExport(exportTypes_, exportData);

			if (optionExport)
				options.AddExport(*optionExport);
			else
			{
				throw OptionsParserException(exportData.exportType +
				                             " is not a valid export type.");
			}
		}
	}

	//----------------------------------------------------------------------------
	void ExportOptionParser::AddOption(
	    boost::program_options::options_description& options)
	{
		options.add_options()(
		    ExportOptionParser::ExportTypeOption.c_str(),
		    boost::program_options::value<std::vector<std::string>>()
		        ->default_value({ExportOptionParser::ExportTypeHtmlValue},
		                        ExportOptionParser::ExportTypeHtmlValue),
		    GetExportTypeText().c_str());
	}
}
