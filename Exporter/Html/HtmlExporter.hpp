#ifndef EXPORTER_HTMLEXPORTER_HEADER_GARD
#define EXPORTER_HTMLEXPORTER_HEADER_GARD

#include "../Export.hpp" // $$ fix

#include "TemplateHtmlExporter.hpp"
#include "HtmlFileCoverageExporter.hpp"
namespace CppCoverage
{
	class CoverageData;
}

namespace boost
{
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
		HtmlExporter(
			const boost::filesystem::path& projectTemplatePath,
			const boost::filesystem::path& moduleTemplatePath);

		void Export(const CppCoverage::CoverageData&, const boost::filesystem::path& outputFolder) const;

	private:
		HtmlExporter(const HtmlExporter&) = delete;
		HtmlExporter& operator=(const HtmlExporter&) = delete;

	private:
		TemplateHtmlExporter exporter_;
		HtmlFileCoverageExporter fileCoverageExporter_;
	};
}

#endif
