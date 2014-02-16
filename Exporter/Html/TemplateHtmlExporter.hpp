#ifndef EXPORTER_TEMPLATEHTMLEXPORTER_HEADER_GARD
#define EXPORTER_TEMPLATEHTMLEXPORTER_HEADER_GARD

#include <memory>

#include <boost/filesystem.hpp>

#include <ctemplate/template.h>

#include "../Export.hpp"

namespace CppCoverage
{
	class CoverageData;
	class ModuleCoverage;
	class FileCoverage;
}

namespace fs = boost::filesystem;

namespace Exporter
{
	class ITemplateExpander;

	class EXPORTER_DLL TemplateHtmlExporter
	{
	public:
		TemplateHtmlExporter(
			const fs::path& projectTemplatePath,
			const fs::path& moduleTemplatePath);

		std::unique_ptr<ctemplate::TemplateDictionary>	
		CreateTemplateDictionary(const std::wstring& title) const;

		void AddFileSectionToDictionary(
			const CppCoverage::FileCoverage& fileCoverage,
			const fs::path& fileOutput,
			int percentCoverage,
			ctemplate::TemplateDictionary& moduleTemplateDictionary) const;

		void AddModuleSectionToDictionary(
			const CppCoverage::ModuleCoverage& moduleCoverage,
			const fs::path& moduleOutput,
			int percentCoverage,
			ctemplate::TemplateDictionary& projectDictionary) const;

		std::string GenerateModuleTemplate(const ctemplate::TemplateDictionary& templateDictionary) const;
		std::string GenerateProjectTemplate(const ctemplate::TemplateDictionary& templateDictionary) const;

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
	};
}

#endif
