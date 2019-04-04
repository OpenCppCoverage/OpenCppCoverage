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
#include <range/v3/algorithm/find_if.hpp>
#include "ProgramOptionsVariablesMap.hpp"
#include "Options.hpp"
#include "OptionsParserException.hpp"
#include "ExportPluginDescription.hpp"
#include "Tools/Tool.hpp"

namespace CppCoverage
{
	namespace
	{
		//---------------------------------------------------------------------
		struct ExportData
		{
			std::wstring argument;
			std::wstring exportType;
		};

		//---------------------------------------------------------------------
		ExportData ParseExportData(const std::wstring& exportStr)
		{
			ExportData exportData;
			auto pos = exportStr.find(ExportOptionParser::ExportSeparator);

			if (pos != std::wstring::npos)
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
		    const std::map<std::wstring, OptionsExportType>& exportTypes,
		    const ExportData& exportData)
		{
			auto it = exportTypes.find(exportData.exportType);

			if (it != exportTypes.end())
			{
				OptionsExportType type = it->second;
				auto exportOutputPath = exportData.argument;

				return OptionsExport{
				    type,
				    std::wstring{exportData.exportType},
				    exportOutputPath.empty()
				        ? std::nullopt
				        : std::make_optional(exportOutputPath)};
			}

			return boost::none;
		}

		//-------------------------------------------------------------------------
		bool TryAddExportPlugin(const std::vector<ExportPluginDescription>&
		                            exportPluginDescriptions,
		                        const ExportData& exportData,
		                        Options& options)
		{
			auto it = ranges::find_if(
			    exportPluginDescriptions, [&](const auto& plugin) {
				    return plugin.GetPluginName() == exportData.exportType;
			    });
			if (it == ranges::end(exportPluginDescriptions))
				return false;

			auto argument = exportData.argument;
			it->CheckArgument(argument);

			options.AddExport(OptionsExport{OptionsExportType::Plugin,
			                                std::wstring{exportData.exportType},
			                                std::move(argument)});
			return true;
		}
	}

	const char ExportOptionParser::ExportSeparator = ':';
	const std::string ExportOptionParser::ExportTypeOption = "export_type";
	const std::string ExportOptionParser::ExportTypeHtmlValue = "html";
	const std::string ExportOptionParser::ExportTypeCoberturaValue =
	    "cobertura";
	const std::string ExportOptionParser::ExportTypeBinaryValue = "binary";

	//-------------------------------------------------------------------------
	ExportOptionParser::ExportOptionParser(
	    std::vector<ExportPluginDescription>&& exportPluginDescriptions)
	    : exportPluginDescriptions_{std::move(exportPluginDescriptions)}
	{
		exportTypes_.emplace(
		    Tools::LocalToWString(ExportOptionParser::ExportTypeHtmlValue),
		    OptionsExportType::Html);
		exportTypes_.emplace(
		    Tools::LocalToWString(ExportOptionParser::ExportTypeCoberturaValue),
		    OptionsExportType::Cobertura);
		exportTypes_.emplace(
		    Tools::LocalToWString(ExportOptionParser::ExportTypeBinaryValue),
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
			auto exportData =
			    ParseExportData(Tools::LocalToWString(exportTypeStr));
			auto optionExport = CreateExport(exportTypes_, exportData);

			if (optionExport)
				options.AddExport(std::move(*optionExport));
			else if (!TryAddExportPlugin(
			             exportPluginDescriptions_, exportData, options))
			{
				throw OptionsParserException(
				    Tools::ToLocalString(exportData.exportType) +
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
		    Tools::ToLocalString(GetExportTypeText()).c_str());
	}

	//----------------------------------------------------------------------------
	std::wstring ExportOptionParser::GetExportTypeText() const
	{
		const auto separator = ", ";
		std::vector<std::pair<std::wstring, std::wstring>> exportArgumentInfos =
		    {{Tools::LocalToWString(ExportOptionParser::ExportTypeHtmlValue),
		      L"output folder (optional)"},
		     {Tools::LocalToWString(
		          ExportOptionParser::ExportTypeCoberturaValue),
		      L"output file (optional)"},
		     {Tools::LocalToWString(ExportOptionParser::ExportTypeBinaryValue),
		      L"output file (optional)"}};
		for (const auto& description : exportPluginDescriptions_)
		{
			exportArgumentInfos.push_back(
			    {description.GetPluginName(),
			     description.GetParameterDescription()});
		}

		std::wstring exports;
		for (const auto& argumentInfos : exportArgumentInfos)
		{
			exports += L"   " + argumentInfos.first + L": " +
			           argumentInfos.second + L'\n';
		}
		return L"Format: <exportType>[:<parameter>].\n"
		       "<exportType> can be:\n" +
		       exports +
		       L"Example: html:MyFolder\\MySubFolder\n"
		       "This flag can have multiple occurrences.";
	}
}