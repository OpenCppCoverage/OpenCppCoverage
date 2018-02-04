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
#include <fstream>

#include "TestHelper/TemporaryPath.hpp"
#include "Exporter/Html/TemplateHtmlExporter.hpp"
#include "Exporter/Html/CTemplate.hpp"
#include "CppCoverage/CoverageRate.hpp"

using namespace Exporter;

namespace ExporterTest
{
	//-------------------------------------------------------------------------
	struct TemplateHtmlExporterTest : public ::testing::Test
	{
	public:
		//---------------------------------------------------------------------
		void AddTag(std::ostream& ostr, const std::string& tag)
		{
			ostr << tag << ":{{" << tag << "}}" << std::endl;
		}

		//---------------------------------------------------------------------
		void AddSection(
			std::ostream& ostr, 
			const std::string& tag, 
			std::function<void ()> sectionCallback)
		{
			ostr << "{{#" << tag << "}}" << std::endl;
			sectionCallback();
			ostr << "{{/" << tag << "}}" << std::endl;
		}

		//---------------------------------------------------------------------
		fs::path CreateMainTemplate()
		{
			fs::path templatePath = output_folder.GetPath() / "template";
			std::ofstream ofs(templatePath.string());
			
			for (const auto& tag : {
				TemplateHtmlExporter::TitleTemplate,
				TemplateHtmlExporter::MainMessageTemplate})
			{
				AddTag(ofs, tag);
			}

			AddSection(ofs, TemplateHtmlExporter::MainTemplateItemSection, [&]()
			{
				for (const auto& tag : {
					TemplateHtmlExporter::TotalLineTemplate,
					TemplateHtmlExporter::ExecutedLineTemplate,
					TemplateHtmlExporter::UnExecutedLineTemplate,
					TemplateHtmlExporter::CoverRateTemplate,
					TemplateHtmlExporter::UncoverRateTemplate })
				{
					AddTag(ofs, tag);
				}

				AddSection(ofs, TemplateHtmlExporter::ItemLinkSection, [&]()
				{
					AddTag(ofs, TemplateHtmlExporter::NameTemplate);
					AddTag(ofs, TemplateHtmlExporter::LinkTemplate);
				});
				AddSection(ofs, TemplateHtmlExporter::ItemNoLinkSection, [&]()
				{
					AddTag(ofs, TemplateHtmlExporter::NameTemplate);
				});
			});
				
			return templatePath;
		}

		//---------------------------------------------------------------------
		fs::path CreateSourceTemplate()
		{
			fs::path templatePath = output_folder.GetPath() / "template";
			std::ofstream ofs(templatePath.string());

			for (const auto& tag : {
				TemplateHtmlExporter::TitleTemplate,
				TemplateHtmlExporter::BodyOnLoadTemplate,
				TemplateHtmlExporter::SourceWarningMessageTemplate,
				TemplateHtmlExporter::CodeTemplate })
			{
				AddTag(ofs, tag);
			}

			return templatePath;
		}

		//---------------------------------------------------------------------
		std::unordered_map<std::string, std::wstring> ReadTemplate(const fs::path& p)
		{
			std::unordered_map<std::string, std::wstring> results;
			std::wifstream ifs(p.string());
			std::wstring line;

			while (std::getline(ifs, line))
			{
				auto pos = line.find(':');
				if (pos != std::string::npos)
				{
					results.emplace(
						std::string{ line.begin(), line.begin() + pos },
						std::wstring{ line.begin() + pos + 1, line.end() } );
				}
			}
			return results;
		}

		TestHelper::TemporaryPath output_folder{ TestHelper::TemporaryPathOption::CreateAsFolder };
	};

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, ProjectTemplate)
	{
		auto mainTemplate = CreateMainTemplate();
		TemplateHtmlExporter exporter{ mainTemplate, mainTemplate };
		const std::wstring title = L"Title";
		const std::wstring message = L"Message";
		auto project = exporter.CreateTemplateDictionary(title, message);

		CppCoverage::CoverageRate coverage{ 10, 20 };
		const fs::path moduleName = L"ModuleName";
		const fs::path moduleOutput = L"ModuleOutput";
		exporter.AddModuleSectionToDictionary(moduleName, coverage, false, &moduleOutput, *project);

		auto outputFile = output_folder.GetPath() / L"project";
		exporter.GenerateProjectTemplate(*project, outputFile);
		auto templateValues = ReadTemplate(outputFile);

		ASSERT_EQ(title, templateValues.at(TemplateHtmlExporter::TitleTemplate));
		ASSERT_EQ(message, templateValues.at(TemplateHtmlExporter::MainMessageTemplate));
		ASSERT_EQ(moduleName, templateValues.at(TemplateHtmlExporter::NameTemplate));
		ASSERT_EQ(moduleOutput, templateValues.at(TemplateHtmlExporter::LinkTemplate));
		ASSERT_EQ(coverage.GetTotalLinesCount(),
			std::stoi(templateValues.at(TemplateHtmlExporter::TotalLineTemplate)));
		ASSERT_EQ(coverage.GetExecutedLinesCount(), 
			std::stoi(templateValues.at(TemplateHtmlExporter::ExecutedLineTemplate)));
		ASSERT_EQ(coverage.GetUnExecutedLinesCount(), 
			std::stoi(templateValues.at(TemplateHtmlExporter::UnExecutedLineTemplate)));
		ASSERT_EQ(std::trunc(coverage.GetRate() * 100), 
			std::stoi(templateValues.at(TemplateHtmlExporter::CoverRateTemplate)));
		ASSERT_EQ(100 - std::trunc(coverage.GetRate() * 100), 
			std::stoi(templateValues.at(TemplateHtmlExporter::UncoverRateTemplate)));
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, ModuleTemplateWithNoLink)
	{
		auto mainTemplate = CreateMainTemplate();
		TemplateHtmlExporter exporter{ mainTemplate, mainTemplate };		
		auto module = exporter.CreateTemplateDictionary(L"Title", L"Message");
		
		const fs::path filename = "filename";
		exporter.AddFileSectionToDictionary(filename,
			CppCoverage::CoverageRate{ 10, 20 }, false, nullptr, *module);
		auto outputFile = output_folder.GetPath() / "module";
		exporter.GenerateModuleTemplate(*module, outputFile);

		auto templateValues = ReadTemplate(outputFile);

		ASSERT_EQ(filename, templateValues.at(TemplateHtmlExporter::NameTemplate));
	}

	//-------------------------------------------------------------------------
	TEST_F(TemplateHtmlExporterTest, FileTemplate)
	{
		auto sourceTemplate = CreateSourceTemplate();
		TemplateHtmlExporter exporter{ sourceTemplate, sourceTemplate };

		auto outputFile = output_folder.GetPath() / "file";
		std::wstring sourceTitle = L"SourceTitle";
		std::wstring sourceContent = L"SourceContent";
		exporter.GenerateSourceTemplate(sourceTitle, sourceContent, true, outputFile);
		auto templateValues = ReadTemplate(outputFile);

		ASSERT_EQ(sourceTitle, templateValues.at(TemplateHtmlExporter::TitleTemplate));
		ASSERT_EQ(sourceContent, templateValues.at(TemplateHtmlExporter::CodeTemplate));
		ASSERT_NE(L"", templateValues.at(TemplateHtmlExporter::BodyOnLoadTemplate));
		ASSERT_EQ(L"", templateValues.at(TemplateHtmlExporter::SourceWarningMessageTemplate));

		exporter.GenerateSourceTemplate(sourceTitle, sourceContent, false, outputFile);
		templateValues = ReadTemplate(outputFile);

		ASSERT_EQ(L"", templateValues.at(TemplateHtmlExporter::BodyOnLoadTemplate));
		ASSERT_NE(L"", templateValues.at(TemplateHtmlExporter::SourceWarningMessageTemplate));
	}
}