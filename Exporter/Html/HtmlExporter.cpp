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
		: exporter_(templateFolder)
		, fileCoverageExporter_()
		, templateFolder_(templateFolder)
	{
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path HtmlExporter::GetDefaultPath(const std::wstring& runningCommandFilename) const
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
		const boost::filesystem::path& outputFolderPrefix) const
	{	
		HtmlFolderStructure htmlFolderStructure{templateFolder_};
		auto mainMessage = GetMainMessage(coverageData);

		auto projectDictionary = exporter_.CreateTemplateDictionary(coverageData.GetName(), mainMessage);
		auto outputFolder = htmlFolderStructure.CreateCurrentRoot(outputFolderPrefix);

		for (const auto& module : coverageData.GetModules())
		{			
			if (module->GetCoverageRate().GetTotalLinesCount())
			{
				const auto& modulePath = module->GetPath();
				auto moduleFilename = module->GetPath().filename();
				auto moduleTemplateDictionary = exporter_.CreateTemplateDictionary(moduleFilename.wstring(), L"");

				auto htmlModulePath = htmlFolderStructure.CreateCurrentModule(modulePath);
				ExportFiles(*module, htmlFolderStructure, *moduleTemplateDictionary);

				exporter_.GenerateModuleTemplate(*moduleTemplateDictionary, htmlModulePath.GetAbsolutePath());				
				exporter_.AddModuleSectionToDictionary(*module, htmlModulePath.GetRelativeLinkPath(), *projectDictionary);
			}
		}

		exporter_.GenerateProjectTemplate(*projectDictionary, outputFolder / L"index.html");
		Tools::ShowOutputMessage(L"Coverage generated in Folder ", outputFolder);
	}	

	//---------------------------------------------------------------------
	void HtmlExporter::ExportFiles(
		const cov::ModuleCoverage& module,
		const HtmlFolderStructure& htmlFolderStructure, 
		ctemplate::TemplateDictionary& moduleTemplateDictionary) const
	{
		for (const auto& file : module.GetFiles())
		{
			boost::optional<fs::path> generatedOutput = ExportFile(htmlFolderStructure, *file);
			exporter_.AddFileSectionToDictionary(*file, generatedOutput.get_ptr(), moduleTemplateDictionary);
		}
	}

	//---------------------------------------------------------------------
	boost::optional<fs::path> HtmlExporter::ExportFile(
		const HtmlFolderStructure& htmlFolderStructure,
		const cov::FileCoverage& fileCoverage) const
	{
		auto htmlFilePath = htmlFolderStructure.GetHtmlFilePath(fileCoverage.GetPath());
		std::wostringstream ostr;

		if (!fileCoverageExporter_.Export(fileCoverage, ostr))
			return boost::optional<fs::path>();
		
		auto title = fileCoverage.GetPath().filename().wstring();
		exporter_.GenerateSourceTemplate(
			title, ostr.str(), htmlFilePath.GetAbsolutePath());

		return htmlFilePath.GetRelativeLinkPath();
	}	
}

