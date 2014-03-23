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
	class EXPORTER_DLL HtmlExporter
	{
	public:
		explicit HtmlExporter(const fs::path& templateFolder);

		void Export(const CppCoverage::CoverageData&, const boost::filesystem::path& outputFolder) const;

	private:
		HtmlExporter(const HtmlExporter&) = delete;
		HtmlExporter& operator=(const HtmlExporter&) = delete;

		boost::optional<fs::path> ExportFile(
			const fs::path& moduleFolderPath,
			const CppCoverage::FileCoverage& fileCoverage) const;

	private:
		TemplateHtmlExporter exporter_;
		HtmlFileCoverageExporter fileCoverageExporter_;
		fs::path templateFolder_;
	};
}


