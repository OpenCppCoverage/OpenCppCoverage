#include "stdafx.h"
#include "HtmlFileCoverageExporter.hpp"

#include <fstream>
#include <boost/filesystem.hpp>

#include "CppCoverage/FileCoverage.hpp"

#include "../ExporterException.hpp"

namespace fs = boost::filesystem;

namespace Exporter
{
	namespace
	{
		//---------------------------------------------------------------------
		std::wstring UpdateLineColor(const std::wstring& line, bool codeHasBeenExecuted)
		{
			std::wstring output;

			if (codeHasBeenExecuted)
				output += HtmlFileCoverageExporter::StyleBackgroundColorExecuted;
			else
				output += HtmlFileCoverageExporter::StyleBackgroundColorUnexecuted;

			output += line;
			output += L"</span>";

			return output;
		}

		const std::wstring StyleBackgroundColor = L"<span style = \"background-color:#";
	}

	const std::wstring HtmlFileCoverageExporter::StyleBackgroundColorExecuted = StyleBackgroundColor + L"dfd" + L"\">";
	const std::wstring HtmlFileCoverageExporter::StyleBackgroundColorUnexecuted = StyleBackgroundColor + L"fdd" + L"\">";
	
	//-------------------------------------------------------------------------
	bool HtmlFileCoverageExporter::Export(
		const CppCoverage::FileCoverage& fileCoverage,
		std::wostream& output) const
	{
		auto filePath = fileCoverage.GetPath();

		if (!fs::exists(filePath))
			return false;

		std::wifstream ifs{filePath.string()};
		if (!ifs)
			THROW(L"Cannot open file : " + filePath.wstring());

		std::wstring line;
		for (size_t i = 1; std::getline(ifs, line); ++i)
		{			
			auto lineCoverage = fileCoverage[i];

			if (lineCoverage)
				line = UpdateLineColor(line, lineCoverage->HasBeenExecuted());
				
			output << line << std::endl;
		}
		output.flush();

		return true;
	}
}
