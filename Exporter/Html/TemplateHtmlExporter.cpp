#include "stdafx.h"
#include "TemplateHtmlExporter.hpp"

#include <boost/filesystem.hpp>
#include <ctemplate/template.h>

#include "Tools/Tool.hpp"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"

#include "../ExporterException.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace Exporter
{
	namespace
	{
		const std::string moduleSection = "MODULE";		
		const std::string fileSection = "FILE";
		const std::string titleTemplate = "TITLE";
		const std::string rateTemplate = "RATE";
		const std::string codeTemplate = "CODE";
		const std::string cssPathTemplate = "CSS_PATH";
		const std::string prettifyPathTemplate = "PRETTIFY_PATH";
	
		//-------------------------------------------------------------------------
		std::string ToStr(const std::wstring& str)
		{
			return Tools::Tool::ToString(str);
		}
		
		//-------------------------------------------------------------------------
		void FillSection(
			ctemplate::TemplateDictionary& sectionDictionary,
			const fs::path* link,
			const cov::CoverageRate& coverageRate,
			const std::string& name)
		{
			if (link)
			{
				Tools::Tool::CheckPathExists(*link);
				sectionDictionary.SetValue(TemplateHtmlExporter::LinkTemplate, link->string());
			}
			sectionDictionary.SetIntValue(rateTemplate, coverageRate.GetPercentRate());
			sectionDictionary.SetIntValue(TemplateHtmlExporter::ExecutedLineTemplate, coverageRate.GetExecutedLinesCount());
			sectionDictionary.SetIntValue(TemplateHtmlExporter::TotalLineTemplate, coverageRate.GetTotalLinesCount());
			sectionDictionary.SetValue(TemplateHtmlExporter::NameTemplate, name);
		}		
	}
	
	//-------------------------------------------------------------------------
	const std::string TemplateHtmlExporter::ExecutedLineTemplate = "EXECUTED_LINE";
	const std::string TemplateHtmlExporter::LinkTemplate = "LINK";
	const std::string TemplateHtmlExporter::TotalLineTemplate = "TOTAL_LINE";	
	const std::string TemplateHtmlExporter::NameTemplate = "NAME";

	//-------------------------------------------------------------------------
	TemplateHtmlExporter::TemplateHtmlExporter(const fs::path& templateFolder)
		: projectTemplatePath_(templateFolder / "IndexTemplate.html")
		, moduleTemplatePath_(templateFolder / "ModuleTemplate.html")
		, fileTemplatePath_(templateFolder / "SourceTemplate.html")		
	{		
	}

	//-------------------------------------------------------------------------
	std::unique_ptr<ctemplate::TemplateDictionary> 
	TemplateHtmlExporter::CreateTemplateDictionary(const std::wstring& title) const
	{
		std::string titleStr{ ToStr(title) };
		std::unique_ptr<ctemplate::TemplateDictionary> dictionary;

		dictionary.reset(new ctemplate::TemplateDictionary(titleStr));

		dictionary->SetValue(titleTemplate, titleStr);

		return dictionary;
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::AddFileSectionToDictionary(
		const cov::FileCoverage& fileCoverage,
		const fs::path* fileOutput,		
		ctemplate::TemplateDictionary& moduleTemplateDictionary) const
	{
		auto sectionDictionary = moduleTemplateDictionary.AddSectionDictionary(fileSection);
		auto filePath = fileCoverage.GetPath();

		FillSection(*sectionDictionary, fileOutput, fileCoverage.GetCoverageRate(), filePath.string());
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::AddModuleSectionToDictionary(			
		const cov::ModuleCoverage& moduleCoverage,
		const fs::path& moduleOutput,
		ctemplate::TemplateDictionary& projectDictionary) const
	{
		auto sectionDictionary = projectDictionary.AddSectionDictionary(moduleSection);
		auto path = moduleCoverage.GetPath();									
				
		FillSection(*sectionDictionary, &moduleOutput, moduleCoverage.GetCoverageRate(), path.string());
	}				

	//-------------------------------------------------------------------------
	std::string TemplateHtmlExporter::GenerateModuleTemplate(
		const ctemplate::TemplateDictionary& templateDictionary) const
	{
		return GenerateTemplate(templateDictionary, moduleTemplatePath_);
	}

	//-------------------------------------------------------------------------
	std::string TemplateHtmlExporter::GenerateProjectTemplate(
		const ctemplate::TemplateDictionary& templateDictionary) const
	{
		return GenerateTemplate(templateDictionary, projectTemplatePath_);
	}

	//-------------------------------------------------------------------------
	std::string TemplateHtmlExporter::GenerateSourceTemplate(
		const std::wstring& title,
		const std::wstring& codeContent,
		const fs::path& codeCss,
		const fs::path& codePrettify) const
	{
		Tools::Tool::CheckPathExists(codeCss);
		Tools::Tool::CheckPathExists(codePrettify);

		auto titleStr = Tools::Tool::ToString(title);
		ctemplate::TemplateDictionary dictionary(titleStr);

		dictionary.SetValue(titleTemplate, titleStr);
		dictionary.SetValue(codeTemplate, Tools::Tool::ToString(codeContent));	
		dictionary.SetValue(cssPathTemplate, codeCss.string());
		dictionary.SetValue(prettifyPathTemplate, codePrettify.string());

		return GenerateTemplate(dictionary, fileTemplatePath_);
	}

	//-------------------------------------------------------------------------
	std::string TemplateHtmlExporter::GenerateTemplate(
		const ctemplate::TemplateDictionary& templateDictionary, 
		const fs::path& templatePath) const
	{
		std::string output;
		
		if (!ctemplate::ExpandTemplate(templatePath.string(), ctemplate::DO_NOT_STRIP, &templateDictionary, &output))
			THROW(L"Cannot generate output for " + templatePath.wstring()); 

		return output;
	}	
}
