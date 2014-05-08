#include "stdafx.h"

#include "Exporter/Html/CTemplate.hpp"
#include "Exporter/Html/TemplateHtmlExporter.hpp"
#include "Exporter/ExporterException.hpp"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "Tools/Tool.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

#include "Tools/TemporaryFolder.hpp"

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
		void CheckSection(const std::string& sectionName)
		{
			std::vector<const ctemplate::TemplateDictionary*> fileTemplates;
			peer_->GetSectionDictionaries(sectionName, &fileTemplates);
			EXPECT_EQ(1, fileTemplates.size());
			ctemplate::TemplateDictionaryPeer fileTemplatePeer{ fileTemplates[0] };

			EXPECT_EQ(fileOutput_.GetPath().string(), fileTemplatePeer.GetSectionValue(
				Exporter::TemplateHtmlExporter::LinkTemplate));
			EXPECT_EQ(std::string("0"), fileTemplatePeer.GetSectionValue(
				Exporter::TemplateHtmlExporter::ExecutedLineTemplate));
			EXPECT_EQ(std::string("0"), fileTemplatePeer.GetSectionValue(
				Exporter::TemplateHtmlExporter::TotalLineTemplate));						
			EXPECT_EQ(filePath_.string(), fileTemplatePeer.GetSectionValue(
				Exporter::TemplateHtmlExporter::NameTemplate));
		}

		const std::wstring title_;
		std::unique_ptr<ctemplate::TemplateDictionaryPeer> peer_;
		std::unique_ptr<ctemplate::TemplateDictionary> templateDictionary_;	
		Exporter::TemplateHtmlExporter templateHtmlExporter_;	
		Tools::TemporaryFolder fileOutput_;
		fs::path filePath_;
	};

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, CreateTemplateDictionary)
	{		
		ASSERT_EQ(Tools::ToString(title_), peer_->GetSectionValue("TITLE"));
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, AddFileSectionToDictionary)
	{			
		CppCoverage::FileCoverage fileCoverage{filePath_};
		templateHtmlExporter_.AddFileSectionToDictionary(fileCoverage, &fileOutput_.GetPath(), *templateDictionary_);
		CheckSection("FILE");
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, AddModuleSectionToDictionary)
	{
		CppCoverage::ModuleCoverage moduleCoverage{ filePath_ };
		templateHtmlExporter_.AddModuleSectionToDictionary(moduleCoverage, fileOutput_.GetPath(), *templateDictionary_);
		CheckSection("MODULE");		
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