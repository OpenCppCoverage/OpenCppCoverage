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

#include <boost/algorithm/string.hpp>
#include "CppCoverage/FileCoverage.hpp"
#include "Exporter/Html/HtmlFileCoverageExporter.hpp"
#include "TestHelper/TemporaryPath.hpp"

namespace ExporterTest
{
	namespace
	{
		enum class CoverageType
		{
			Cover,
			UnCover,
			NotExecutable
		};

		using SourceLines = std::vector<std::pair<std::wstring, CoverageType>>;

		//-----------------------------------------------------------------
		void FillSources(
			const SourceLines& sourceLines,
			const TestHelper::TemporaryPath& sourceFile,
			CppCoverage::FileCoverage& fileCoverage)
		{
			std::wofstream ofs(sourceFile.GetPath().wstring());

			unsigned int lineNumber = 1;
			for (const auto& line : sourceLines)
			{
				ofs << line.first << std::endl;
				auto coverageType = line.second;
				switch (coverageType)
				{
					case CoverageType::Cover: fileCoverage.AddLine(lineNumber, true); break;
					case CoverageType::UnCover: fileCoverage.AddLine(lineNumber, false); break;
					case CoverageType::NotExecutable: break;
				}
				++lineNumber;
			}
		}

		//-----------------------------------------------------------------
		std::vector<std::wstring> GetExportedLines(
			const SourceLines& sourceLines)
		{
			std::wostringstream ostr;
			TestHelper::TemporaryPath sourceFile;
			CppCoverage::FileCoverage fileCoverage{ sourceFile };

			FillSources(sourceLines, sourceFile, fileCoverage);

			if (!Exporter::HtmlFileCoverageExporter{}.Export(fileCoverage, ostr))
				throw std::runtime_error("Error in HtmlFileCoverageExporter::Export");

			std::wstring exportedString = ostr.str();
			std::vector<std::wstring> lines;
			boost::split(lines, exportedString, boost::is_any_of("\n"));

			// First line is always empty
			lines.erase(lines.begin());

			if (lines.size() != sourceLines.size())
				throw std::runtime_error("Invalid number of exported lines.");
			return lines;
		}
	}

	const auto StyleExecuted = Exporter::HtmlFileCoverageExporter::StyleBackgroundColorExecuted;
	const auto StyleNotExecuted = Exporter::HtmlFileCoverageExporter::StyleBackgroundColorUnexecuted;
	const auto EndStyle = Exporter::HtmlFileCoverageExporter::EndStyle;
	const std::wstring Line = L"line";

	//---------------------------------------------------------------------
	TEST(HtmlFileCoverageExporterTest, ExecutedLine)
	{
		auto exportedLines = GetExportedLines({ {Line, CoverageType::Cover} });
		ASSERT_EQ(StyleExecuted + Line + EndStyle, exportedLines.at(0));
	}

	//---------------------------------------------------------------------
	TEST(HtmlFileCoverageExporterTest, NotExecutedLine)
	{
		auto exportedLines = GetExportedLines({ { Line, CoverageType::UnCover } });
		ASSERT_EQ(StyleNotExecuted + Line + EndStyle, exportedLines.at(0));
	}

	//---------------------------------------------------------------------
	TEST(HtmlFileCoverageExporterTest, NotRunnableLine)
	{
		auto exportedLines = GetExportedLines({ { Line, CoverageType::NotExecutable } });
		ASSERT_EQ(Line, exportedLines.at(0));
	}

	//---------------------------------------------------------------------
	TEST(HtmlFileCoverageExporterTest, SeveralLines)
	{
		std::vector<std::wstring> lines = { L"1", L"2", L"3", L"4", L"5", L"6" };
		auto exportedLines = GetExportedLines({
			{ lines.at(0), CoverageType::UnCover },
			{ lines.at(1), CoverageType::UnCover },
			{ lines.at(2), CoverageType::NotExecutable }, 
			{ lines.at(3), CoverageType::NotExecutable },
			{ lines.at(4), CoverageType::Cover },
			{ lines.at(5), CoverageType::Cover } });
				
		ASSERT_EQ(StyleNotExecuted + lines.at(0), exportedLines.at(0));
		ASSERT_EQ(lines.at(1) + EndStyle, exportedLines.at(1));
		ASSERT_EQ(lines.at(2), exportedLines.at(2));
		ASSERT_EQ(lines.at(3), exportedLines.at(3));
		ASSERT_EQ(StyleExecuted + lines.at(4), exportedLines.at(4));
		ASSERT_EQ(lines.at(5) + EndStyle, exportedLines.at(5));
	}

	//---------------------------------------------------------------------
	TEST(HtmlFileCoverageExporterTest, MustEnableCodePrettify)
	{
		Exporter::HtmlFileCoverageExporter exporter;

		ASSERT_TRUE(exporter.MustEnableCodePrettify(8000, 1000));
		ASSERT_FALSE(exporter.MustEnableCodePrettify(8001, 1000));
		ASSERT_FALSE(exporter.MustEnableCodePrettify(8000, 1001));
		
		ASSERT_TRUE(exporter.MustEnableCodePrettify(4000, 1666));
		ASSERT_FALSE(exporter.MustEnableCodePrettify(4000, 1667));

		ASSERT_TRUE(exporter.MustEnableCodePrettify(2000, 2000));
		ASSERT_FALSE(exporter.MustEnableCodePrettify(2000, 2001));
	}
}