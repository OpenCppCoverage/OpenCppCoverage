#ifndef EXPORTER_HTMLFILECOVERAGEEXPORTER_HEADER_GARD
#define EXPORTER_HTMLFILECOVERAGEEXPORTER_HEADER_GARD

#include <iosfwd> 

#include "../Export.hpp"

namespace CppCoverage
{
	class FileCoverage;
}

namespace Exporter
{
	class EXPORTER_DLL HtmlFileCoverageExporter
	{
	public:
		HtmlFileCoverageExporter() = default;

		void Export(
			const CppCoverage::FileCoverage&, 
			std::wostream& output) const;

	private:
		HtmlFileCoverageExporter(const HtmlFileCoverageExporter&) = delete;
		HtmlFileCoverageExporter& operator=(const HtmlFileCoverageExporter&) = delete;
	};
}

#endif
