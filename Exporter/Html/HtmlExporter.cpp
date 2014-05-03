#include "stdafx.h"
#include "HtmlExporter.hpp"

#include <sstream>
#include <ctemplate/template.h>

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"

#include "Tools/Log.hpp"

#include "TemplateHtmlExporter.hpp"
#include "HtmlFileCoverageExporter.hpp"
#include "HtmlFolderStructure.hpp"
namespace cov = CppCoverage;

namespace Exporter
{		

	namespace
	{		
		//-------------------------------------------------------------------------
		void WriteContentTo(const std::string& content, const fs::path& path)
		{
			std::ofstream ofs(path.string());

			ofs << content;
			ofs.flush();
		}

		//---------------------------------------------------------------------
		void ShowOutputMessage(const fs::path& outputFolder)
		{
			auto separators = L"----------------------------------------------------";
			LOG_INFO << separators;
			LOG_INFO << L"Coverage generated in Folder " << outputFolder.wstring();
			LOG_INFO << separators;
		}
	}
	
	//-------------------------------------------------------------------------
	HtmlExporter::HtmlExporter(const fs::path& templateFolder)
		: exporter_(templateFolder)
		, fileCoverageExporter_()
		, templateFolder_(templateFolder)
	{
	}

	//-------------------------------------------------------------------------
	void HtmlExporter::Export(
		const CppCoverage::CoverageData& coverageData, 
		const boost::filesystem::path& outputFolderPrefix) const
	{	
		HtmlFolderStructure htmlFolderStructure{templateFolder_};

		auto projectDictionary = exporter_.CreateTemplateDictionary(coverageData.GetName());				
		auto outputFolder = htmlFolderStructure.CreateCurrentRoot(outputFolderPrefix);

		for (const auto& module : coverageData.GetModules())
		{			
			const auto& modulePath = module->GetPath();
			auto moduleFilename = module->GetPath().filename();			
			auto moduleTemplateDictionary = exporter_.CreateTemplateDictionary(moduleFilename.wstring());
			
			auto htmlModulePath = htmlFolderStructure.CreateCurrentModule(modulePath);
			ExportFiles(*module, htmlFolderStructure, *moduleTemplateDictionary);
			
			auto content = exporter_.GenerateModuleTemplate(*moduleTemplateDictionary);			
			WriteContentTo(content, htmlModulePath);
			exporter_.AddModuleSectionToDictionary(*module, htmlModulePath, *projectDictionary);
		}

		auto content = exporter_.GenerateProjectTemplate(*projectDictionary);
		WriteContentTo(content, outputFolder / L"index.html");
		ShowOutputMessage(outputFolder);
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
		auto content = exporter_.GenerateSourceTemplate(
			title, ostr.str(), htmlFolderStructure.GetCodeCssPath(), htmlFolderStructure.GetCodePrettifyPath());

		WriteContentTo(content, htmlFilePath);

		return htmlFilePath;
	}	
}

