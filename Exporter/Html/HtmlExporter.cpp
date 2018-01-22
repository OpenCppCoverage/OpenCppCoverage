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
#include "HtmlExporter.hpp"

#include <sstream>
#include <iomanip>
#include "CTemplate.hpp"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/CoverageRateComputer.hpp"
#include "CppCoverage/CoverageRate.hpp"

#include "Tools/Log.hpp"
#include "Tools/Tool.hpp"

#include "TemplateHtmlExporter.hpp"
#include "HtmlFileCoverageExporter.hpp"
#include "HtmlFolderStructure.hpp"
namespace cov = CppCoverage;

namespace Exporter
{		

	namespace
	{
		//-------------------------------------------------------------------------
		std::wstring GetMainMessage(const CppCoverage::CoverageData& coverageData)
		{
			auto exitCode = coverageData.GetExitCode();

			if (exitCode)
				return HtmlExporter::WarningExitCodeMessage + std::to_wstring(exitCode);
			return L"";
		}
	}
	
	//-------------------------------------------------------------------------
	const std::wstring HtmlExporter::WarningExitCodeMessage = L"Warning: Your program has exited with error code: ";

	//-------------------------------------------------------------------------
	HtmlExporter::HtmlExporter(const fs::path& templateFolder)
		: exporter_(templateFolder / "MainTemplate.html", templateFolder / "SourceTemplate.html")
		, fileCoverageExporter_()
		, templateFolder_(templateFolder)
	{
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path HtmlExporter::GetDefaultPath(const std::wstring&) const
	{
		auto now = std::time(nullptr);
		auto localNow = std::localtime(&now);
		std::ostringstream ostr;

		ostr << "CoverageReport-" << std::put_time(localNow, "%Y-%m-%d-%Hh%Mm%Ss");

		return ostr.str();
	}

	//-------------------------------------------------------------------------
	void HtmlExporter::Export(
		const CppCoverage::CoverageData& coverageData, 
		const boost::filesystem::path& outputFolderPrefix)
	{	
		HtmlFolderStructure htmlFolderStructure{templateFolder_};
		cov::CoverageRateComputer coverageRateComputer{ coverageData };

		auto mainMessage = GetMainMessage(coverageData);

		auto projectDictionary = exporter_.CreateTemplateDictionary(coverageData.GetName(), mainMessage);
		auto outputFolder = htmlFolderStructure.CreateCurrentRoot(outputFolderPrefix);

		exporter_.AddModuleSectionToDictionary(
		    coverageData.GetName(),
		    coverageRateComputer.GetCoverageRate(),
			true,
			nullptr,
		    *projectDictionary);

		for (const auto& module : coverageRateComputer.SortModulesByCoverageRate())
		{			
			const auto& moduleCoverageRate = coverageRateComputer.GetCoverageRate(*module);

			if (moduleCoverageRate.GetTotalLinesCount())
			{
				const auto& modulePath = module->GetPath();
				auto moduleFilename = module->GetPath().filename();
				auto moduleTemplateDictionary = exporter_.CreateTemplateDictionary(moduleFilename.wstring(), L"");

				auto htmlModulePath = htmlFolderStructure.CreateCurrentModule(modulePath);
				ExportFiles(coverageRateComputer, *module, htmlFolderStructure, *moduleTemplateDictionary);

				exporter_.GenerateModuleTemplate(*moduleTemplateDictionary, htmlModulePath.GetAbsolutePath());
				exporter_.AddModuleSectionToDictionary(
				    module->GetPath(),
				    moduleCoverageRate,
					false,
				    &htmlModulePath.GetRelativeLinkPath(),
				    *projectDictionary);
			}
		}

		exporter_.GenerateProjectTemplate(*projectDictionary, outputFolder / L"index.html");
		Tools::ShowOutputMessage(L"Coverage generated in Folder ", outputFolder);
	}	

	//---------------------------------------------------------------------
	void HtmlExporter::ExportFiles(
		cov::CoverageRateComputer& coverageRateComputer,
		const cov::ModuleCoverage& module,
		const HtmlFolderStructure& htmlFolderStructure, 
		ctemplate::TemplateDictionary& moduleTemplateDictionary)
	{
		exporter_.AddFileSectionToDictionary(
			module.GetPath(),
			coverageRateComputer.GetCoverageRate(module),
			true,
			nullptr, 
			moduleTemplateDictionary);

		for (const auto& file : coverageRateComputer.SortFilesByCoverageRate(module))
		{
			const auto& fileCoverageRate = coverageRateComputer.GetCoverageRate(*file);
			boost::optional<fs::path> generatedOutput = ExportFile(htmlFolderStructure, *file);
			exporter_.AddFileSectionToDictionary(
				file->GetPath(), 
				fileCoverageRate, 
				false,
				generatedOutput.get_ptr(), 
				moduleTemplateDictionary);
		}
	}

	//---------------------------------------------------------------------
	boost::optional<fs::path> HtmlExporter::ExportFile(
		const HtmlFolderStructure& htmlFolderStructure,
		const cov::FileCoverage& fileCoverage) const
	{
		auto htmlFilePath = htmlFolderStructure.GetHtmlFilePath(fileCoverage.GetPath());
		std::wostringstream ostr;
		
		if (!fs::exists(fileCoverage.GetPath()))
			return boost::optional<fs::path>();

		auto enableCodePrettify = fileCoverageExporter_.Export(fileCoverage, ostr);

		auto title = fileCoverage.GetPath().filename().wstring();
		exporter_.GenerateSourceTemplate(
			title, ostr.str(), enableCodePrettify, htmlFilePath.GetAbsolutePath());

		return htmlFilePath.GetRelativeLinkPath();
	}	
}

