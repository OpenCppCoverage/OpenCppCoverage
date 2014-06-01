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

#include "Exporter/Html/CTemplate.hpp"
#include "Exporter/Html/TemplateHtmlExporter.hpp"
#include "Exporter/ExporterException.hpp"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "Tools/Tool.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

#include "Tools/TemporaryPath.hpp"

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
						
			EXPECT_EQ(fileOutput_.GetPath().string(), itemLink->GetSectionValue(
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

		const std::wstring title_;
		std::unique_ptr<ctemplate::TemplateDictionaryPeer> peer_;
		std::unique_ptr<ctemplate::TemplateDictionary> templateDictionary_;	
		Exporter::TemplateHtmlExporter templateHtmlExporter_;	
		Tools::TemporaryPath fileOutput_;
		fs::path filePath_;
	};

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, CreateTemplateDictionary)
	{		
		ASSERT_EQ(Tools::ToString(title_), peer_->GetSectionValue(
			Exporter::TemplateHtmlExporter::TitleTemplate));
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, AddFileSectionToDictionary)
	{			
		CppCoverage::FileCoverage fileCoverage{filePath_};
		templateHtmlExporter_.AddFileSectionToDictionary(fileCoverage, &fileOutput_.GetPath(), *templateDictionary_);
		CheckItems();
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, AddModuleSectionToDictionary)
	{
		CppCoverage::ModuleCoverage moduleCoverage{ filePath_ };
		templateHtmlExporter_.AddModuleSectionToDictionary(moduleCoverage, fileOutput_.GetPath(), *templateDictionary_);
		CheckItems();
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, AddFileSectionToDictionaryNoLink)
	{
		CppCoverage::FileCoverage fileCoverage{ filePath_ };
		templateHtmlExporter_.AddFileSectionToDictionary(fileCoverage, nullptr, *templateDictionary_);
		auto itemSection = GetSection(*peer_, Exporter::TemplateHtmlExporter::MainTemplateItemSection);

		ASSERT_EQ(0, GetSectionDictionaries(*itemSection, 
			Exporter::TemplateHtmlExporter::ItemLinkSection).size());
		ASSERT_EQ(1, GetSectionDictionaries(*itemSection, 
			Exporter::TemplateHtmlExporter::ItemNoLinkSection).size());
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, FileExists)
	{		
		CppCoverage::ModuleCoverage moduleCoverage{ filePath_ };
		templateHtmlExporter_.AddModuleSectionToDictionary(moduleCoverage, ".", *templateDictionary_);			
		templateHtmlExporter_.GenerateModuleTemplate(*templateDictionary_, fileOutput_.GetPath());

		ASSERT_TRUE(fs::exists(fileOutput_.GetPath()));
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, FileNotExists)
	{
		CppCoverage::ModuleCoverage moduleCoverage{ filePath_ };
		templateHtmlExporter_.AddModuleSectionToDictionary(moduleCoverage, "MissingFile", *templateDictionary_);

		ASSERT_THROW(templateHtmlExporter_.GenerateModuleTemplate(*templateDictionary_, fileOutput_.GetPath()), Exporter::ExporterException);
	}

} 