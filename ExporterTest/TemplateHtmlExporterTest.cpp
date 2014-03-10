#include "stdafx.h"

#include <tests/template_test_util.h>

#include "Exporter/Html/TemplateHtmlExporter.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "Tools/Tool.hpp"

namespace cov = CppCoverage;

namespace ExporterTest
{
	struct TemplateHtmlExporterTest : public ::testing::Test
	{
	public:
		TemplateHtmlExporterTest()
			: templateHtmlExporter_("")
			, title_(L"title")
			, fileOutput_{ "FilePath" }
			, filePath_{ "path" }
		{			
			templateDictionary_ = templateHtmlExporter_.CreateTemplateDictionary(title_);
			peer_.reset(new ctemplate::TemplateDictionaryPeer(templateDictionary_.get()));
		}
			// $$ tester le code avec des exceptions pour voir si cela fonctionne 
		void CheckSection(const std::string& sectionName)
		{
			std::vector<const ctemplate::TemplateDictionary*> fileTemplates;
			peer_->GetSectionDictionaries(sectionName, &fileTemplates);
			EXPECT_EQ(1, fileTemplates.size());
			ctemplate::TemplateDictionaryPeer fileTemplatePeer{ fileTemplates[0] };

			EXPECT_EQ(fileOutput_.string(), fileTemplatePeer.GetSectionValue("LINK"));
			EXPECT_EQ(std::string("0"), fileTemplatePeer.GetSectionValue("EXECUTED_LINE")); // $$ use constantes here
			EXPECT_EQ(std::string("0"), fileTemplatePeer.GetSectionValue("UNEXECUTED_LINE")); // $$ use constantes here
						
			EXPECT_EQ(filePath_.string(), fileTemplatePeer.GetSectionValue("NAME"));
		}

		const std::wstring title_;
		std::unique_ptr<ctemplate::TemplateDictionaryPeer> peer_;
		std::unique_ptr<ctemplate::TemplateDictionary> templateDictionary_;	
		Exporter::TemplateHtmlExporter templateHtmlExporter_;	
		fs::path fileOutput_;
		fs::path filePath_;
	};

	TEST_F(TemplateHtmlExporterTest, CreateTemplateDictionary)
	{		
		ASSERT_EQ(Tools::Tool::ToString(title_), peer_->GetSectionValue("TITLE"));
	}

	TEST_F(TemplateHtmlExporterTest, AddFileSectionToDictionary)
	{			
		CppCoverage::FileCoverage fileCoverage{filePath_};
		templateHtmlExporter_.AddFileSectionToDictionary(fileCoverage, &fileOutput_, *templateDictionary_);
		CheckSection("FILE");
	}

	TEST_F(TemplateHtmlExporterTest, AddModuleSectionToDictionary)
	{
		CppCoverage::ModuleCoverage moduleCoverage{ filePath_ };
		templateHtmlExporter_.AddModuleSectionToDictionary(moduleCoverage, fileOutput_, *templateDictionary_);
		CheckSection("MODULE");		
	}

}