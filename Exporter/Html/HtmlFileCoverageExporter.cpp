// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "HtmlFileCoverageExporter.hpp"

#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_tree_to_xml.hpp>

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
			
			line = boost::spirit::classic::xml::encode(line);
				
			if (lineCoverage)
				line = UpdateLineColor(line, lineCoverage->HasBeenExecuted());
				
			output << line << std::endl;
		}
		output.flush();

		return true;
	}
}
