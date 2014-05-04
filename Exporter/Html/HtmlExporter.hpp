#pragma once

#include "../ExporterExport.hpp"

#include "TemplateHtmlExporter.hpp"
#include "HtmlFileCoverageExporter.hpp"

namespace CppCoverage
{
	class CoverageData;
	class FileCoverage;
}

namespace boost
{
	template <typename T>
	class optional;

	namespace filesystem
	{
		class path;
	}
}

namespace Exporter
{
	class HtmlFolderStructure;

	class EXPORTER_DLL HtmlExporter
	{
	public:
		static const std::wstring WarningExitCodeMessage;

	public:
		explicit HtmlExporter(const boost::filesystem::path& templateFolder);

		void Export(const CppCoverage::CoverageData&, const boost::filesystem::path& outputFolder) const;

	private:
		HtmlExporter(const HtmlExporter&) = delete;
		HtmlExporter& operator=(const HtmlExporter&) = delete;

		boost::optional<boost::filesystem::path> ExportFile(
			const HtmlFolderStructure& htmlFolderStructure,
			const CppCoverage::FileCoverage& fileCoverage) const;

		void ExportFiles(
			const CppCoverage::ModuleCoverage& module,
			const HtmlFolderStructure& htmlFolderStructure,
			ctemplate::TemplateDictionary& moduleTemplateDictionary) const;

	private:
		TemplateHtmlExporter exporter_;
		HtmlFileCoverageExporter fileCoverageExporter_;
		boost::filesystem::path templateFolder_;
	};
}


