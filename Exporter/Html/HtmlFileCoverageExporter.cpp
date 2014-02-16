#include "stdafx.h"
#include "HtmlFileCoverageExporter.hpp"

#include <fstream>
#include <boost/filesystem.hpp>

#include "CppCoverage/FileCoverage.hpp"

namespace fs = boost::filesystem;

namespace Exporter
{
	//-------------------------------------------------------------------------
	void HtmlFileCoverageExporter::Export(
		const CppCoverage::FileCoverage& fileCoverage,
		std::wostream& output) const
	{
		auto filePath = fileCoverage.GetPath();
		std::wifstream ifs{filePath.string()};

		if (!ifs)
			throw L"Cannot open file : " + filePath.wstring(); //$$ todod
		if (!output)
			throw L"Output is not valid"; //$$ todod
				
		std::wstring line;
		for (size_t i = 0; std::getline(ifs, line); ++i)
		{			
			auto lineCoverage = fileCoverage[i];
			std::wstring coverageResults;

			if (lineCoverage)
				coverageResults = (lineCoverage->HasBeenExecuted()) ? L"OK: ": L"KO: ";
			else
				coverageResults = L"    ";
			output << coverageResults << line << std::endl;
		}
		output.flush();
	}
}
