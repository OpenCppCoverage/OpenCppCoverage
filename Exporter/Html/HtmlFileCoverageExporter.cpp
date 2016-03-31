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
namespace cov = CppCoverage;

namespace Exporter
{
	namespace
	{
		//---------------------------------------------------------------------
		bool HaveSameCoverage(
			const cov::LineCoverage* lineCoverage,
			const cov::LineCoverage* otherLineCoverage)
		{
			if (!lineCoverage || !otherLineCoverage)
				return lineCoverage == otherLineCoverage;
			return lineCoverage->HasBeenExecuted() == otherLineCoverage->HasBeenExecuted();
		}

		//---------------------------------------------------------------------
		std::wstring GetStyle(const cov::LineCoverage* lineCoverage)
		{
			if (!lineCoverage)
				return L"";

			return (lineCoverage->HasBeenExecuted())
				? HtmlFileCoverageExporter::StyleBackgroundColorExecuted
				: HtmlFileCoverageExporter::StyleBackgroundColorUnexecuted;
		}

		//---------------------------------------------------------------------
		void AddEndStyleIfNeeded(
			std::wostream& output,
			const cov::LineCoverage* previousLineCoverage)
		{
			if (previousLineCoverage)
				output << HtmlFileCoverageExporter::EndStyle;
		}

		//---------------------------------------------------------------------
		void AddLineCoverageColor(
			std::wostream& output,
			const std::wstring& line, 
			const cov::LineCoverage* lineCoverage,
			const cov::LineCoverage* previousLineCoverage)
		{
			if (HaveSameCoverage(lineCoverage, previousLineCoverage))
				output << std::endl << line;
			else
			{
				AddEndStyleIfNeeded(output, previousLineCoverage);
				output << std::endl;
				output << GetStyle(lineCoverage) << line;
			}
		}

		const std::wstring StyleBackgroundColor = L"<span style = \"background-color:#";
	}

	const std::wstring HtmlFileCoverageExporter::StyleBackgroundColorExecuted = 
		StyleBackgroundColor + L"dfd" + L"\">";
	const std::wstring HtmlFileCoverageExporter::StyleBackgroundColorUnexecuted = 
		StyleBackgroundColor + L"fdd" + L"\">";
	const std::wstring HtmlFileCoverageExporter::EndStyle = L"</span>";

	//-------------------------------------------------------------------------
	HtmlFileCoverageExporter::HtmlFileCoverageExporter() = default;

	//-------------------------------------------------------------------------
	bool HtmlFileCoverageExporter::Export(
		const cov::FileCoverage& fileCoverage,
		std::wostream& output) const
	{
		auto filePath = fileCoverage.GetPath();

		if (!fs::exists(filePath))
			return false;

		std::wifstream ifs{filePath.string()};
		if (!ifs)
			THROW(L"Cannot open file : " + filePath.wstring());

		std::wstring line;
		const cov::LineCoverage* previousLineCoverage = nullptr;
		for (int i = 1; std::getline(ifs, line); ++i)
		{			
			auto lineCoverage = fileCoverage[i];
			
			line = boost::spirit::classic::xml::encode(line);				
			AddLineCoverageColor(output, line, lineCoverage, previousLineCoverage);
			previousLineCoverage = lineCoverage;
		}
		AddEndStyleIfNeeded(output, previousLineCoverage);
		output.flush();

		return true;
	}
}
