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

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "Exporter/Html/CTemplate.hpp"
#include "Exporter/Html/TemplateHtmlExporter.hpp"
#include "Exporter/ExporterException.hpp"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/CoverageRate.hpp"
#include "Tools/Tool.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

#include "TestHelper/TemporaryPath.hpp"

namespace cov = CppCoverage;

namespace ExporterTest
{
	//-------------------------------------------------------------------------
	struct TemplateHtmlExporterTest : public ::testing::Test
	{
	public:
		//---------------------------------------------------------------------
		TemplateHtmlExporterTest()
			: templateHtmlExporter_(Tools::GetTemplateFolder())
			, title_(L"title")
			, fileOutput_{}
			, filePath_{ "path" }
		{			
			templateDictionary_ = templateHtmlExporter_.CreateTemplateDictionary(title_, L"");
			peer_.reset(new ctemplate::TemplateDictionaryPeer(templateDictionary_.get()));
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

		//---------------------------------------------------------------------
		std::unique_ptr<ctemplate::TemplateDictionaryPeer> GetSection(
			const ctemplate::TemplateDictionaryPeer& peer,
			const std::string& sectionName)
		{									
			auto dictionary = GetSectionDictionaries(peer, sectionName);
			return std::unique_ptr<ctemplate::TemplateDictionaryPeer>{new ctemplate::TemplateDictionaryPeer{ dictionary.at(0) }};
		}

		//---------------------------------------------------------------------
		void CheckItems()
		{
			auto fileTemplatePeer = GetSection(*peer_, Exporter::TemplateHtmlExporter::MainTemplateItemSection);			
			auto itemLink = GetSection(*fileTemplatePeer, Exporter::TemplateHtmlExporter::ItemLinkSection);
			auto htmlPath = fileOutput_.GetPath().string();

			boost::algorithm::replace_all(htmlPath, "\\", "/");
			EXPECT_EQ(htmlPath, itemLink->GetSectionValue(
				Exporter::TemplateHtmlExporter::LinkTemplate));
			EXPECT_EQ(std::string("0"), fileTemplatePeer->GetSectionValue(
				Exporter::TemplateHtmlExporter::ExecutedLineTemplate));
			EXPECT_EQ(std::string("0"), fileTemplatePeer->GetSectionValue(
				Exporter::TemplateHtmlExporter::UnExecutedLineTemplate));
			EXPECT_EQ(std::string("0"), fileTemplatePeer->GetSectionValue(
				Exporter::TemplateHtmlExporter::TotalLineTemplate));						
			EXPECT_EQ(filePath_.string(), fileTemplatePeer->GetSectionValue(
				Exporter::TemplateHtmlExporter::NameTemplate));
		}

		//---------------------------------------------------------------------
		std::wstring GenerateSourceTemplate(
			const std::wstring& title,
			const std::wstring& codeContent,
			bool enableCodePrettify) const
		{
			TestHelper::TemporaryPath path;
			templateHtmlExporter_.GenerateSourceTemplate(title, codeContent, enableCodePrettify, path);
			std::wifstream ifs(path.GetPath().string());

			return{ std::istreambuf_iterator<wchar_t>(ifs), std::istreambuf_iterator<wchar_t>() };
		}

		const std::wstring title_;
		std::unique_ptr<ctemplate::TemplateDictionaryPeer> peer_;
		std::unique_ptr<ctemplate::TemplateDictionary> templateDictionary_;	
		Exporter::TemplateHtmlExporter templateHtmlExporter_;	
		TestHelper::TemporaryPath fileOutput_;
		fs::path filePath_;
		cov::CoverageRate coverageRate_;
	};

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, CreateTemplateDictionary)
	{		
		ASSERT_EQ(Tools::ToLocalString(title_), peer_->GetSectionValue(
			Exporter::TemplateHtmlExporter::TitleTemplate));
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, AddFileSectionToDictionary)
	{					
		templateHtmlExporter_.AddFileSectionToDictionary(filePath_, coverageRate_, 
			&fileOutput_.GetPath(), *templateDictionary_);
		CheckItems();
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, AddModuleSectionToDictionary)
	{	
		templateHtmlExporter_.AddModuleSectionToDictionary(filePath_, coverageRate_, 
			fileOutput_.GetPath(), *templateDictionary_);
		CheckItems();
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, AddFileSectionToDictionaryNoLink)
	{		
		templateHtmlExporter_.AddFileSectionToDictionary(filePath_, coverageRate_,
			nullptr, *templateDictionary_);
		auto itemSection = GetSection(*peer_, Exporter::TemplateHtmlExporter::MainTemplateItemSection);

		ASSERT_EQ(0, GetSectionDictionaries(*itemSection, 
			Exporter::TemplateHtmlExporter::ItemLinkSection).size());
		ASSERT_EQ(1, GetSectionDictionaries(*itemSection, 
			Exporter::TemplateHtmlExporter::ItemNoLinkSection).size());
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, FileExists)
	{		
		templateHtmlExporter_.AddModuleSectionToDictionary(filePath_, coverageRate_,
			".", *templateDictionary_);
		templateHtmlExporter_.GenerateModuleTemplate(*templateDictionary_, fileOutput_.GetPath());

		ASSERT_TRUE(fs::exists(fileOutput_.GetPath()));
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, FileNotExists)
	{
		templateHtmlExporter_.AddModuleSectionToDictionary(filePath_, coverageRate_,
			"MissingFile", *templateDictionary_);

		ASSERT_THROW(templateHtmlExporter_.GenerateModuleTemplate(*templateDictionary_, fileOutput_.GetPath()), Exporter::ExporterException);
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, GenerateSourceTemplate)
	{
		const std::wstring title = L"TITLE";
		const std::wstring codeContent = L"CODE_CONTENT";
		
		auto output = GenerateSourceTemplate(title, codeContent, true);

		ASSERT_TRUE(boost::algorithm::contains(output, title));
		ASSERT_TRUE(boost::algorithm::contains(output, codeContent));
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, PrettifyEnabled)
	{
		auto output = GenerateSourceTemplate(L"", L"", true);
		
		ASSERT_TRUE(boost::algorithm::contains(output, Exporter::TemplateHtmlExporter::BodyOnLoadFct));
		ASSERT_FALSE(boost::algorithm::contains(output, Exporter::TemplateHtmlExporter::SyntaxHighlightingDisabledMsg));
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, PrettifyDisabled)
	{
		auto output = GenerateSourceTemplate(L"", L"", false);

		ASSERT_FALSE(boost::algorithm::contains(output, Exporter::TemplateHtmlExporter::BodyOnLoadFct));
		ASSERT_TRUE(boost::algorithm::contains(output, Exporter::TemplateHtmlExporter::SyntaxHighlightingDisabledMsg));
	}
} 