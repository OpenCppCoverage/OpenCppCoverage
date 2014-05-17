#include "stdafx.h"
#include "TemplateHtmlExporter.hpp"

#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <boost/lexical_cast.hpp>

#include "CTemplate.hpp"
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
		const std::string mainTemplateItemSection = "ITEMS";		
		const std::string titleTemplate = "TITLE";
		const std::string coverRateTemplate = "COVER_RATE";
		const std::string uncoverRateTemplate = "UNCOVER_RATE";
		const std::string codeTemplate = "CODE";				
		const std::string messageTemplate = "MAIN_MESSAGE";
		const std::string idTemplate = "ID";
		const std::string thirdPartyPathTemplate = "THIRD_PARTY_PATH";


		//-------------------------------------------------------------------------
		std::string ToStr(const std::wstring& str)
		{
			return Tools::ToString(str);
		}
		
		//-------------------------------------------------------------------------
		std::string GetUuid()
		{
			boost::uuids::random_generator generator;
			boost::uuids::uuid id(generator());
			
			return boost::uuids::to_string(id);
		}

		//-------------------------------------------------------------------------
		void FillSection(
			ctemplate::TemplateDictionary& sectionDictionary,
			const fs::path* link,
			const cov::CoverageRate& coverageRate,
			const std::string& name)
		{
			if (link)
				sectionDictionary.SetValue(TemplateHtmlExporter::LinkTemplate, link->string());
			
			sectionDictionary.SetIntValue(coverRateTemplate, coverageRate.GetPercentRate());
			sectionDictionary.SetIntValue(uncoverRateTemplate, 100 - coverageRate.GetPercentRate());
			sectionDictionary.SetIntValue(TemplateHtmlExporter::ExecutedLineTemplate, coverageRate.GetExecutedLinesCount());
			sectionDictionary.SetIntValue(TemplateHtmlExporter::UnExecutedLineTemplate, coverageRate.GetUnExecutedLinesCount());
			sectionDictionary.SetIntValue(TemplateHtmlExporter::TotalLineTemplate, coverageRate.GetTotalLinesCount());			
			sectionDictionary.SetValue(idTemplate, GetUuid());
			sectionDictionary.SetValue(TemplateHtmlExporter::NameTemplate, name);				
		}	
		
		//-------------------------------------------------------------------------
		void WriteContentTo(const std::string& content, const fs::path& path)
		{
			std::ofstream ofs(path.string());

			if (!ofs)
				THROW(L"Cannot open file" << path);
			ofs << content;
			ofs.flush();
		}

		//-------------------------------------------------------------------------
		std::string GenerateTemplate(
			const ctemplate::TemplateDictionary& templateDictionary,
			const fs::path& templatePath)
		{
			std::string output;

			if (!ctemplate::ExpandTemplate(templatePath.string(), ctemplate::DO_NOT_STRIP, &templateDictionary, &output))
				THROW(L"Cannot generate output for " + templatePath.wstring());

			return output;
		}

		//-------------------------------------------------------------------------
		void CheckLinkExists(
			const ctemplate::TemplateDictionary& templateDictionary,
			const std::string& sectionName,
			const fs::path& output)
		{
			ctemplate::TemplateDictionaryPeer rootPeer(&templateDictionary);
			std::vector<const ctemplate::TemplateDictionary*> dicts;
			rootPeer.GetSectionDictionaries(sectionName, &dicts);

			for (const auto& dictionary : dicts)
			{
				ctemplate::TemplateDictionaryPeer peer(dicts[0]);
				auto linkStr = peer.GetSectionValue(TemplateHtmlExporter::LinkTemplate);

				if (linkStr)
				{
					auto fullPath = output.parent_path() / linkStr;

					if (!fs::exists(fullPath))
						THROW("Link: " << fullPath << " does not exists");
				}
			}
		}

		//-------------------------------------------------------------------------
		void WriteTemplate(
			const ctemplate::TemplateDictionary& templateDictionary,
			const fs::path& templatePath,
			const fs::path& output)
		{			
			std::string content = GenerateTemplate(templateDictionary, templatePath);
			WriteContentTo(content, output);
		}
	}
	
	//-------------------------------------------------------------------------
	const std::string TemplateHtmlExporter::ExecutedLineTemplate = "EXECUTED_LINE";
	const std::string TemplateHtmlExporter::UnExecutedLineTemplate = "UNEXECUTED_LINE";
	const std::string TemplateHtmlExporter::LinkTemplate = "LINK";
	const std::string TemplateHtmlExporter::TotalLineTemplate = "TOTAL_LINE";	
	const std::string TemplateHtmlExporter::NameTemplate = "NAME";

	//-------------------------------------------------------------------------
	TemplateHtmlExporter::TemplateHtmlExporter(const fs::path& templateFolder)
		: mainTemplatePath_(templateFolder / "MainTemplate.html")		
		, fileTemplatePath_(templateFolder / "SourceTemplate.html")		
	{		
	}

	//-------------------------------------------------------------------------
	std::unique_ptr<ctemplate::TemplateDictionary> 
	TemplateHtmlExporter::CreateTemplateDictionary(
		const std::wstring& title, 
		const std::wstring& message) const
	{
		std::string titleStr{ ToStr(title) };
		std::unique_ptr<ctemplate::TemplateDictionary> dictionary;

		dictionary.reset(new ctemplate::TemplateDictionary(titleStr));

		dictionary->SetValue(titleTemplate, titleStr);
		dictionary->SetValue(messageTemplate, ToStr(message));

		return dictionary;
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::AddFileSectionToDictionary(
		const cov::FileCoverage& fileCoverage,
		const fs::path* fileOutput,		
		ctemplate::TemplateDictionary& moduleTemplateDictionary) const
	{
		auto sectionDictionary = moduleTemplateDictionary.AddSectionDictionary(mainTemplateItemSection);
		auto filePath = fileCoverage.GetPath();

		moduleTemplateDictionary.SetValue(thirdPartyPathTemplate, "../third-party");
		FillSection(*sectionDictionary, fileOutput, fileCoverage.GetCoverageRate(), filePath.string());
	}
	
	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::AddModuleSectionToDictionary(			
		const cov::ModuleCoverage& moduleCoverage,
		const fs::path& moduleOutput,
		ctemplate::TemplateDictionary& projectDictionary) const
	{
		auto sectionDictionary = projectDictionary.AddSectionDictionary(mainTemplateItemSection);
		auto path = moduleCoverage.GetPath();									
			
		projectDictionary.SetValue(thirdPartyPathTemplate, "third-party");
		FillSection(*sectionDictionary, &moduleOutput, moduleCoverage.GetCoverageRate(), path.string());
	}				
	
	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::GenerateModuleTemplate(
		const ctemplate::TemplateDictionary& templateDictionary,
		const fs::path& output) const
	{
		CheckLinkExists(templateDictionary, mainTemplateItemSection, output);
		WriteTemplate(templateDictionary, mainTemplatePath_, output);
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::GenerateProjectTemplate(
		const ctemplate::TemplateDictionary& templateDictionary,
		const fs::path& output) const
	{
		CheckLinkExists(templateDictionary, mainTemplateItemSection, output);
		WriteTemplate(templateDictionary, mainTemplatePath_, output);
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::GenerateSourceTemplate(
		const std::wstring& title,
		const std::wstring& codeContent,		
		const fs::path& output) const
	{
		auto titleStr = Tools::ToString(title);
		ctemplate::TemplateDictionary dictionary(titleStr);

		dictionary.SetValue(titleTemplate, titleStr);
		dictionary.SetValue(codeTemplate, Tools::ToString(codeContent));	
		WriteTemplate(dictionary, fileTemplatePath_, output);
	}	
}
