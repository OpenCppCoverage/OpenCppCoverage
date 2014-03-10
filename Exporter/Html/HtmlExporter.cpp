#include "stdafx.h"
#include "HtmlExporter.hpp"

#include <sstream>
#include <ctemplate/template.h>

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"

#include "TemplateHtmlExporter.hpp"
#include "HtmlFileCoverageExporter.hpp"

namespace cov = CppCoverage;

namespace Exporter
{		

	namespace
	{
		const std::wstring codePrettify = L"google-code-prettify";

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

		void RecursiveDirectoryContent(
			const fs::path& from,
			const fs::path& to)
		{
			fs::create_directory(to);
			for (fs::recursive_directory_iterator it(from); it != fs::recursive_directory_iterator(); ++it)
			{
				const auto& path = it->path();

				fs::copy_file(path, to / path.filename());
					
				// $$std::cout << to / path.filename() << std::endl;
			}
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
		const boost::filesystem::path& outputFolder) const
	{				
		auto projectDictionary = exporter_.CreateTemplateDictionary(coverageData.GetName());

		fs::create_directory(outputFolder); // $$ manage conflit
		RecursiveDirectoryContent(templateFolder_ / codePrettify, outputFolder / codePrettify);
		for (const auto& module : coverageData.GetModules())
		{			
			auto moduleFilename = module->GetPath().filename();
			auto moduleFolderPath = outputFolder / moduleFilename.replace_extension("");
			auto moduleTemplateDictionary = exporter_.CreateTemplateDictionary(moduleFilename.wstring());
			
			fs::create_directory(moduleFolderPath);
			for (const auto& file : module->GetFiles())
			{
				boost::optional<fs::path> generatedOutput = ExportFile(moduleFolderPath, *file);
				exporter_.AddFileSectionToDictionary(*file, generatedOutput.get_ptr(), *moduleTemplateDictionary);
			}

			auto modulePath = moduleFolderPath.replace_extension(L".html"); // $$ manage collision
			auto content = exporter_.GenerateModuleTemplate(*moduleTemplateDictionary);			
			WriteContentTo(content, modulePath);
			exporter_.AddModuleSectionToDictionary(*module, modulePath, *projectDictionary);
		}
		
		auto content = exporter_.GenerateProjectTemplate(*projectDictionary);
		WriteContentTo(content, outputFolder / L"index.html");
	}	

	//---------------------------------------------------------------------
	boost::optional<fs::path> HtmlExporter::ExportFile(
		const fs::path& moduleFolderPath, 
		const cov::FileCoverage& fileCoverage) const
	{
		auto fileOutput = GetFileCoverageOutput(moduleFolderPath, fileCoverage);
		std::wostringstream ostr;

		if (!fileCoverageExporter_.Export(fileCoverage, ostr))
			return boost::optional<fs::path>();
		
		auto title = fileCoverage.GetPath().filename().wstring();
		auto content = exporter_.GenerateSourceTemplate(title, ostr.str());
		WriteContentTo(content, fileOutput);
		return fileOutput;
	}
}

