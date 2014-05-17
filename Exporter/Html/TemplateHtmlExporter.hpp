#pragma once

#include <memory>

#include <boost/filesystem.hpp>
#include "../ExporterExport.hpp"

namespace CppCoverage
{
	class CoverageData;
	class ModuleCoverage;
	class FileCoverage;
}

namespace ctemplate
{
	class TemplateDictionary;
}

namespace fs = boost::filesystem;

namespace Exporter
{
	class ITemplateExpander;

	class EXPORTER_DLL TemplateHtmlExporter
	{
	public:
		static const std::string ExecutedLineTemplate;
		static const std::string TitleTemplate;
		static const std::string MainTemplateItemSection;
		static const std::string UnExecutedLineTemplate;
		static const std::string LinkTemplate;
		static const std::string TotalLineTemplate;		
		static const std::string NameTemplate;
		static const std::string ItemLinkSection;
		static const std::string ItemNoLinkSection;

	public:
		explicit TemplateHtmlExporter(const fs::path& templateFolder);

		std::unique_ptr<ctemplate::TemplateDictionary>	
		CreateTemplateDictionary(const std::wstring& title, const std::wstring& message) const;

		void AddFileSectionToDictionary(
			const CppCoverage::FileCoverage& fileCoverage,
			const fs::path* fileOutput,
			ctemplate::TemplateDictionary& moduleTemplateDictionary) const;

		void AddModuleSectionToDictionary(
			const CppCoverage::ModuleCoverage& moduleCoverage,
			const fs::path& moduleOutput,
			ctemplate::TemplateDictionary& projectDictionary) const;

		void GenerateModuleTemplate(
			const ctemplate::TemplateDictionary& templateDictionary,
			const fs::path&) const;

		void GenerateProjectTemplate(
			const ctemplate::TemplateDictionary& templateDictionary,
			const fs::path&) const;

		void GenerateSourceTemplate(
			const std::wstring& title, 
			const std::wstring& codeContent,
			const fs::path& output) const;

	private:
		TemplateHtmlExporter(const TemplateHtmlExporter&) = delete;
		TemplateHtmlExporter& operator=(const TemplateHtmlExporter&) = delete;

	private:
		fs::path mainTemplatePath_;		
		fs::path fileTemplatePath_;
	};
}


