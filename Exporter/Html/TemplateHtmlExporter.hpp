#pragma once

#include <memory>

#include <boost/filesystem.hpp>
#include "../Export.hpp"

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
		explicit TemplateHtmlExporter(const fs::path& templateFolder);

		std::unique_ptr<ctemplate::TemplateDictionary>	
		CreateTemplateDictionary(const std::wstring& title) const;

		void AddFileSectionToDictionary(
			const CppCoverage::FileCoverage& fileCoverage,
			const fs::path* fileOutput,
			ctemplate::TemplateDictionary& moduleTemplateDictionary) const;

		void AddModuleSectionToDictionary(
			const CppCoverage::ModuleCoverage& moduleCoverage,
			const fs::path& moduleOutput,
			ctemplate::TemplateDictionary& projectDictionary) const;

		std::string GenerateModuleTemplate(const ctemplate::TemplateDictionary& templateDictionary) const;
		std::string GenerateProjectTemplate(const ctemplate::TemplateDictionary& templateDictionary) const;
		std::string GenerateSourceTemplate(const std::wstring& title, const std::wstring& codeContent) const;

	private:

		std::string GenerateTemplate(
			const ctemplate::TemplateDictionary& templateDictionary, 
			const fs::path& templatePath) const;

	private:
		TemplateHtmlExporter(const TemplateHtmlExporter&) = delete;
		TemplateHtmlExporter& operator=(const TemplateHtmlExporter&) = delete;

	private:
		fs::path projectTemplatePath_;
		fs::path moduleTemplatePath_;
		fs::path fileTemplatePath_;
	};
}


