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
#include "TemplateHtmlExporter.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "CTemplate.hpp"
#include "Tools/Tool.hpp"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/CoverageRate.hpp"

#include "../ExporterException.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace Exporter
{
	namespace
	{				
		const std::string coverRateTemplate = "COVER_RATE";
		const std::string uncoverRateTemplate = "UNCOVER_RATE";
		const std::string codeTemplate = "CODE";				
		const std::string messageTemplate = "MAIN_MESSAGE";
		const std::string idTemplate = "ID";
		const std::string thirdPartyPathTemplate = "THIRD_PARTY_PATH";
		
		//-------------------------------------------------------------------------
		std::string ToHtmlPath(const fs::path& path)
		{
			auto htmlPath = path.string();
			boost::algorithm::replace_all(htmlPath, "\\", "/");
			return htmlPath;
		}

		//-------------------------------------------------------------------------
		void WriteContentTo(const std::string& content, const fs::path& path)
		{
			std::ofstream ofs(path.string(), std::ios::binary);

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

		//---------------------------------------------------------------------
		std::vector<const ctemplate::TemplateDictionary*> GetSectionDictionaries(
			const ctemplate::TemplateDictionaryPeer& peer,
			const std::string& sectionName)
		{
			std::vector<const ctemplate::TemplateDictionary*> templateDictionaries;
			peer.GetSectionDictionaries(sectionName, &templateDictionaries);

			return templateDictionaries;
		}
		
		//-------------------------------------------------------------------------
		void CheckLinkExists(
			const ctemplate::TemplateDictionary& templateDictionary,
			const std::string& sectionName,
			const fs::path& output)
		{
			ctemplate::TemplateDictionaryPeer rootPeer(&templateDictionary);			
			auto dictionaries = GetSectionDictionaries(rootPeer, sectionName);

			for (const auto& dictionary : dictionaries)
			{
				ctemplate::TemplateDictionaryPeer peer(dictionary);
				auto itemLinkDictionaries = GetSectionDictionaries(peer, TemplateHtmlExporter::ItemLinkSection);

				if (!itemLinkDictionaries.empty())
				{
					ctemplate::TemplateDictionaryPeer itemLinkPeer(itemLinkDictionaries[0]);
					auto linkStr = itemLinkPeer.GetSectionValue(TemplateHtmlExporter::LinkTemplate);

					if (linkStr)
					{
						auto fullPath = output.parent_path() / linkStr;

						if (!fs::exists(fullPath))
							THROW("Link: " << fullPath << " does not exists");
					}
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
	const std::string TemplateHtmlExporter::MainTemplateItemSection = "ITEMS";
	const std::string TemplateHtmlExporter::TitleTemplate = "TITLE";
	const std::string TemplateHtmlExporter::ExecutedLineTemplate = "EXECUTED_LINE";
	const std::string TemplateHtmlExporter::UnExecutedLineTemplate = "UNEXECUTED_LINE";
	const std::string TemplateHtmlExporter::LinkTemplate = "LINK";
	const std::string TemplateHtmlExporter::TotalLineTemplate = "TOTAL_LINE";	
	const std::string TemplateHtmlExporter::NameTemplate = "NAME";
	const std::string TemplateHtmlExporter::ItemLinkSection = "ITEM_LINK";
	const std::string TemplateHtmlExporter::ItemNoLinkSection = "ITEM_NO_LINK";
	const std::string TemplateHtmlExporter::BodyOnLoadTemplate = "BODY_ON_LOAD";
	const std::string TemplateHtmlExporter::SourceWarningMessageTemplate = "SOURCE_WARNING_MESSAGE";
	const std::string TemplateHtmlExporter::BodyOnLoadFct = "prettyPrint()";
	const std::string TemplateHtmlExporter::SyntaxHighlightingDisabledMsg 
		= "Syntax highlighting has been disabled for performance reasons.";

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
		std::string titleStr{ Tools::ToLocalString(title) };
		std::unique_ptr<ctemplate::TemplateDictionary> dictionary;

		dictionary.reset(new ctemplate::TemplateDictionary(titleStr));

		dictionary->SetValue(TitleTemplate, titleStr);
		dictionary->SetValue(messageTemplate, Tools::ToLocalString(message));

		return dictionary;
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::AddFileSectionToDictionary(
		const fs::path& originalFilename,
		const cov::CoverageRate& coverageRate,
		const fs::path* fileOutput,		
		ctemplate::TemplateDictionary& moduleTemplateDictionary)
	{
		auto sectionDictionary = moduleTemplateDictionary.AddSectionDictionary(MainTemplateItemSection);
		
		moduleTemplateDictionary.SetValue(thirdPartyPathTemplate, "../third-party");
		FillSection(*sectionDictionary, fileOutput, coverageRate, originalFilename.string());
	}
	
	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::AddModuleSectionToDictionary(			
		const fs::path& originalFilename,
		const cov::CoverageRate& coverageRate,
		const fs::path& moduleOutput,
		ctemplate::TemplateDictionary& projectDictionary)
	{
		auto sectionDictionary = projectDictionary.AddSectionDictionary(MainTemplateItemSection);			
			
		projectDictionary.SetValue(thirdPartyPathTemplate, "third-party");
		FillSection(*sectionDictionary, &moduleOutput, coverageRate, originalFilename.string());
	}				
	
	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::GenerateModuleTemplate(
		const ctemplate::TemplateDictionary& templateDictionary,
		const fs::path& output) const
	{
		CheckLinkExists(templateDictionary, MainTemplateItemSection, output);
		WriteTemplate(templateDictionary, mainTemplatePath_, output);
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::GenerateProjectTemplate(
		const ctemplate::TemplateDictionary& templateDictionary,
		const fs::path& output) const
	{
		CheckLinkExists(templateDictionary, MainTemplateItemSection, output);
		WriteTemplate(templateDictionary, mainTemplatePath_, output);
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::GenerateSourceTemplate(
		const std::wstring& title,
		const std::wstring& codeContent,
		bool enableCodePrettify,
		const fs::path& output) const
	{
		auto titleStr = Tools::ToLocalString(title);
		ctemplate::TemplateDictionary dictionary(titleStr);
		std::string bodyLoad = TemplateHtmlExporter::BodyOnLoadFct;
		std::string warning = "";

		if (!enableCodePrettify)
		{
			bodyLoad = "";
			warning = TemplateHtmlExporter::SyntaxHighlightingDisabledMsg;
		}

		dictionary.SetValue(TitleTemplate, titleStr);
		dictionary.SetValue(codeTemplate, Tools::ToLocalString(codeContent));
		dictionary.SetValue(BodyOnLoadTemplate, bodyLoad);
		dictionary.SetValue(SourceWarningMessageTemplate, warning);
		WriteTemplate(dictionary, fileTemplatePath_, output);
	}
	//-------------------------------------------------------------------------
	std::string TemplateHtmlExporter::GetUuid()
	{
		boost::uuids::uuid id(uuidGenerator_());

		return boost::uuids::to_string(id);
	}

	//-------------------------------------------------------------------------
	void TemplateHtmlExporter::FillSection(
		ctemplate::TemplateDictionary& sectionDictionary,
		const fs::path* link,
		const cov::CoverageRate& coverageRate,
		const std::string& name)
	{
		if (link)
		{
			auto htmlPath = ToHtmlPath(link->string());
			sectionDictionary.SetValueAndShowSection(
				TemplateHtmlExporter::LinkTemplate, 
				htmlPath, 
				TemplateHtmlExporter::ItemLinkSection);
		}
		else
			sectionDictionary.ShowSection(TemplateHtmlExporter::ItemNoLinkSection);

		sectionDictionary.SetIntValue(coverRateTemplate, coverageRate.GetPercentRate());
		sectionDictionary.SetIntValue(uncoverRateTemplate, 100 - coverageRate.GetPercentRate());
		sectionDictionary.SetIntValue(TemplateHtmlExporter::ExecutedLineTemplate, coverageRate.GetExecutedLinesCount());
		sectionDictionary.SetIntValue(TemplateHtmlExporter::UnExecutedLineTemplate, coverageRate.GetUnExecutedLinesCount());
		sectionDictionary.SetIntValue(TemplateHtmlExporter::TotalLineTemplate, coverageRate.GetTotalLinesCount());
		sectionDictionary.SetValue(idTemplate, GetUuid());
		sectionDictionary.SetValue(TemplateHtmlExporter::NameTemplate, name);
	}
}
