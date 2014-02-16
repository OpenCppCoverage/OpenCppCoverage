#include "stdafx.h"
#include "TemplateHtmlExporter.hpp"

#include <boost/filesystem.hpp>

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/Tools.hpp"
#include "CppCoverage/FileCoverage.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace Exporter
{
	namespace
	{
		const std::string moduleSection = "MODULE";
		const std::string titleSection = "TITLE";
		const std::string fileSection = "FILE";
		const std::string linkTemplate = "LINK";
		const std::string rateTemplate = "RATE";
		const std::string nameTemplate = "NAME";


		//-------------------------------------------------------------------------
		std::string ToStr(const std::wstring& str)
		{
			return cov::Tools::ToString(str);
		}

		//-------------------------------------------------------------------------
		void FillSection(
			ctemplate::TemplateDictionary& sectionDictionary,
			const std::string& link,
			int percentCoverage,
			const std::string& name)
		{
			sectionDictionary.SetValue(linkTemplate, link);
			sectionDictionary.SetIntValue(rateTemplate, percentCoverage);
			sectionDictionary.SetValue(nameTemplate, name);
		}
	}
	

	//-------------------------------------------------------------------------
	TemplateHtmlExporter::TemplateHtmlExporter(
		const fs::path& projectTemplatePath,
		const fs::path& moduleTemplatePath)
		: projectTemplatePath_(projectTemplatePath)
		, moduleTemplatePath_(moduleTemplatePath)
	{
	}

	//-------------------------------------------------------------------------
	std::unique_ptr<ctemplate::TemplateDictionary> 
	TemplateHtmlExporter::CreateTemplateDictionary(const std::wstring& title) const
	{
		std::string titleStr{ ToStr(title) };
		std::unique_ptr<ctemplate::TemplateDictionary> dictionary;

		dictionary.reset(new ctemplate::TemplateDictionary(titleStr));

		dictionary->SetValue(titleSection, titleStr);

		return dictionary;
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::AddFileSectionToDictionary(
		const cov::FileCoverage& fileCoverage,
		const fs::path& fileOutput,
		int percentCoverage,
		ctemplate::TemplateDictionary& moduleTemplateDictionary) const
	{
		auto sectionDictionary = moduleTemplateDictionary.AddSectionDictionary(fileSection);
		auto filePath = fileCoverage.GetPath();

		FillSection(*sectionDictionary, fileOutput.string(), percentCoverage, filePath.string());
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::AddModuleSectionToDictionary(			
		const cov::ModuleCoverage& moduleCoverage,
		const fs::path& moduleOutput,
		int percentCoverage,
		ctemplate::TemplateDictionary& projectDictionary) const
	{
		auto sectionDictionary = projectDictionary.AddSectionDictionary(moduleSection);
		auto path = moduleCoverage.GetPath();									
				
		FillSection(*sectionDictionary, moduleOutput.string(), percentCoverage, path.string());
	}				

	//-------------------------------------------------------------------------
	std::string TemplateHtmlExporter::GenerateModuleTemplate(const ctemplate::TemplateDictionary& templateDictionary) const
	{
		return GenerateTemplate(templateDictionary, moduleTemplatePath_);
	}

	//-------------------------------------------------------------------------
	std::string TemplateHtmlExporter::GenerateProjectTemplate(const ctemplate::TemplateDictionary& templateDictionary) const
	{
		return GenerateTemplate(templateDictionary, projectTemplatePath_);
	}

	//-------------------------------------------------------------------------
	std::string TemplateHtmlExporter::GenerateTemplate(
		const ctemplate::TemplateDictionary& templateDictionary, 
		const fs::path& templatePath) const
	{
		std::string output;
		
		if (!ctemplate::ExpandTemplate(templatePath.string(), ctemplate::DO_NOT_STRIP, &templateDictionary, &output))
			throw (L"Cannot generate output for " + templatePath.wstring()); //$$ use Exception
		return output;
	}
}
