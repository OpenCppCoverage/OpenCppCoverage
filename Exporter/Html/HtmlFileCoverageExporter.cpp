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
#include <filesystem>
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_tree_to_xml.hpp>

#include "Plugin/Exporter/FileCoverage.hpp"

#include "../ExporterException.hpp"

namespace fs = std::filesystem;

namespace Exporter
{
	namespace
	{
		//---------------------------------------------------------------------
		bool HaveSameCoverage(
			const Plugin::LineCoverage* lineCoverage,
			const Plugin::LineCoverage* otherLineCoverage)
		{
			if (!lineCoverage || !otherLineCoverage)
				return lineCoverage == otherLineCoverage;
			return lineCoverage->HasBeenExecuted() == otherLineCoverage->HasBeenExecuted();
		}

		//---------------------------------------------------------------------
		std::wstring GetStyle(const Plugin::LineCoverage* lineCoverage)
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
			const Plugin::LineCoverage* previousLineCoverage)
		{
			if (previousLineCoverage)
				output << HtmlFileCoverageExporter::EndStyle;
		}

		//---------------------------------------------------------------------
		bool AddLineCoverageColor(
			std::wostream& output,
			const std::wstring& line, 
			const Plugin::LineCoverage* lineCoverage,
			const Plugin::LineCoverage* previousLineCoverage)
		{
			if (HaveSameCoverage(lineCoverage, previousLineCoverage))
			{
				output << std::endl << line;
				return false;
			}
			
			AddEndStyleIfNeeded(output, previousLineCoverage);
			auto style = GetStyle(lineCoverage);

			output << std::endl;			
			output << style << line;

			return !style.empty();
		}

		const std::wstring StyleBackgroundColor = L"<span style = \"background-color:#";
	}

	const std::wstring HtmlFileCoverageExporter::StyleBackgroundColorExecuted = 
		StyleBackgroundColor + L"dfd" + L"\">";
	const std::wstring HtmlFileCoverageExporter::StyleBackgroundColorUnexecuted = 
		StyleBackgroundColor + L"fdd" + L"\">";
	const std::wstring HtmlFileCoverageExporter::EndStyle = L"</span>";

	//-------------------------------------------------------------------------
	HtmlFileCoverageExporter::HtmlFileCoverageExporter(
		int maxSourceLineCount,
		int maxSourceLineStyleChangesCount,
		int maxStyleChangesCount)
		: maxSourceLineCount_{ maxSourceLineCount }
		, maxSourceLineStyleChangesCount_{ maxSourceLineStyleChangesCount }
		, maxStyleChangesCount_{ maxStyleChangesCount }
	{
	}

	//-------------------------------------------------------------------------
	bool HtmlFileCoverageExporter::Export(
		const Plugin::FileCoverage& fileCoverage,
		std::wostream& output) const
	{
		auto filePath = fileCoverage.GetPath();

		std::wifstream ifs{filePath.string()};
		if (!ifs)
			THROW(L"Cannot open file : " + filePath.wstring());
		ifs.imbue(std::locale("", LC_CTYPE));

		std::wstring line;
		const Plugin::LineCoverage* previousLineCoverage = nullptr;
		int styleChangesCount = 0;
		int lineCount = 0;
		for (int i = 1; std::getline(ifs, line); ++i)
		{			
			auto lineCoverage = fileCoverage[i];
			
			line = boost::spirit::classic::xml::encode(line);				
			if (AddLineCoverageColor(output, line, lineCoverage, previousLineCoverage))
				++styleChangesCount;
			++lineCount;
			previousLineCoverage = lineCoverage;
		}
		AddEndStyleIfNeeded(output, previousLineCoverage);
		output.flush();

		return MustEnableCodePrettify(lineCount, styleChangesCount);
	}

	//-------------------------------------------------------------------------
	bool HtmlFileCoverageExporter::MustEnableCodePrettify(
		int lineCount,
		int styleChangesCount) const
	{
		if (lineCount > maxSourceLineCount_)
			return false;

		auto ratio = static_cast<double>(maxSourceLineStyleChangesCount_ - maxStyleChangesCount_) 
			/ (maxSourceLineCount_ - maxStyleChangesCount_);
		auto maxStyleChanges = maxStyleChangesCount_ + (lineCount - maxStyleChangesCount_) * ratio;
		
		return styleChangesCount <= maxStyleChanges;
	}
}
