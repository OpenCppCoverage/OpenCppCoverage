#include "stdafx.h"
#include "HtmlExporter.hpp"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"

#include "TemplateHtmlExporter.hpp"
#include "HtmlFileCoverageExporter.hpp"

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
		fs::path GetFileCoverageOutput(
			const fs::path& moduleFolderPath, 
			const CppCoverage::FileCoverage& file)
		{
			auto filename = file.GetPath().filename();
			auto output = filename.replace_extension("html");
			
			return moduleFolderPath / output;
		}
	}
	
	//-------------------------------------------------------------------------
	HtmlExporter::HtmlExporter(
		const fs::path& projectTemplatePath,
		const fs::path& moduleTemplatePath)
		: exporter_(projectTemplatePath, moduleTemplatePath)
		, fileCoverageExporter_()
	{
	}

	//-------------------------------------------------------------------------
	void HtmlExporter::Export(
		const CppCoverage::CoverageData& coverageData, 
		const boost::filesystem::path& outputFolder) const
	{				
		auto projectDictionary = exporter_.CreateTemplateDictionary(coverageData.GetName());

		for (const auto& module : coverageData.GetModules())
		{			
			auto moduleFilename = module->GetPath().filename();
			auto moduleFolderPath = outputFolder / moduleFilename.replace_extension("");
			auto moduleTemplateDictionary = exporter_.CreateTemplateDictionary(moduleFilename.wstring());
			
			fs::create_directory(moduleFolderPath);
			for (const auto& file : module->GetFiles())
			{
				auto fileOutput = GetFileCoverageOutput(moduleFolderPath, *file);
				std::wofstream output{fileOutput.c_str()};

				fileCoverageExporter_.Export(*file, output);
				exporter_.AddFileSectionToDictionary(*file, fileOutput, 42, *moduleTemplateDictionary);
			}

			auto modulePath = moduleFolderPath.replace_extension(L".html"); // $$ manage collision
			auto content = exporter_.GenerateModuleTemplate(*moduleTemplateDictionary);			
			WriteContentTo(content, modulePath);
			exporter_.AddModuleSectionToDictionary(*module, modulePath, 42, *projectDictionary);
		}
		
		auto content = exporter_.GenerateProjectTemplate(*projectDictionary);
		WriteContentTo(content, outputFolder / L"index.html");
	}	
}

