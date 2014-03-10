#pragma once

#include <iosfwd> 

#include "../Export.hpp"

namespace CppCoverage
{
	class FileCoverage;
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
	class EXPORTER_DLL HtmlFileCoverageExporter
	{
	public:
		static const std::wstring StyleBackgroundColorExecuted;
		static const std::wstring StyleBackgroundColorUnexecuted;

	public:
		HtmlFileCoverageExporter() = default;

		bool Export(
			const CppCoverage::FileCoverage&,
			std::wostream& output) const;
		
	private:
		HtmlFileCoverageExporter(const HtmlFileCoverageExporter&) = delete;
		HtmlFileCoverageExporter& operator=(const HtmlFileCoverageExporter&) = delete;
	};
}


