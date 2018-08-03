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

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"

#include "Exporter/InvalidOutputFileException.hpp"
#include "Exporter/SonarQubeExporter.hpp"
#include "tools/Tool.hpp"

#include "TestHelper/TemporaryPath.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace ExporterTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		std::wstring GetExpectedResult()
		{
			fs::path expectedResult = fs::path(PROJECT_DIR) / "Data" / "SonarQubeExporterExpectedResult.xml";
			std::wifstream ifs{ expectedResult.wstring().c_str() };
			std::wostringstream ostr;
			ostr << ifs.rdbuf();
			return ostr.str();
		}
	}

	//-------------------------------------------------------------------------
	TEST(SonarQubeExporterTest, Export)
	{
		cov::CoverageData coverageData{ L"", 0 };
		coverageData.AddModule(L"EmptyModule");

		auto& module = coverageData.AddModule(L"Module");
		module.AddFile("EmptyFile");

		auto& file = module.AddFile("File");
		file.AddLine(0, true);
		file.AddLine(1, false);

		module.AddFile("Directory/File2").AddLine(0, true);

		std::wostringstream ostr;
		Exporter::SonarQubeExporter().Export(coverageData, ostr);
		auto result = ostr.str();
		auto expectedResult = GetExpectedResult();
		ASSERT_EQ(result, expectedResult);
	}

	//-------------------------------------------------------------------------
	TEST(SonarQubeExporterTest, SubFolderDoesNotExist)
	{
		cov::CoverageData coverageData{ L"", 0 };
		TestHelper::TemporaryPath output;
		auto outputPath = output.GetPath() / "SubFolder" / "output.xml";
		ASSERT_FALSE(fs::exists(outputPath));
		Exporter::SonarQubeExporter().Export(coverageData, outputPath);
		ASSERT_TRUE(fs::exists(outputPath));
	}

	//-------------------------------------------------------------------------
	TEST(SonarQubeExporterTest, SpecialChars)
	{
		cov::CoverageData coverageData{ L"", 0 };
		coverageData.AddModule(L"יא").AddFile(L"אי/יא").AddLine(0, true);
		std::wostringstream ostr;
		Exporter::SonarQubeExporter().Export(coverageData, ostr);
		auto result = ostr.str();
		auto filename = L"\u00e0\u00e9\u002f\u00e9\u00e0"; // אי/יא in UTF-8
		ASSERT_TRUE(boost::algorithm::contains(result, filename));
	}

	//-------------------------------------------------------------------------
	TEST(SonarQubeExporterTest, OutputExists)
	{
		cov::CoverageData coverageData{ L"", 0 };
		TestHelper::TemporaryPath outputPath{ TestHelper::TemporaryPathOption::CreateAsFile };
		ASSERT_NO_THROW(Exporter::SonarQubeExporter().Export(coverageData, outputPath));
	}

	//-------------------------------------------------------------------------
	TEST(SonarQubeExporterTest, InvalidFile)
	{
		cov::CoverageData coverageData{ L"", 0 };
		TestHelper::TemporaryPath outputPath{
			TestHelper::TemporaryPathOption::CreateAsFolder };
		ASSERT_THROW(Exporter::SonarQubeExporter().Export(
			coverageData, outputPath.GetPath() / "InvalidFile/"),
			Exporter::InvalidOutputFileException);
	}

	//-------------------------------------------------------------------------
	TEST(SonarQubeExporterTest, PathCase)
	{
		cov::CoverageData coverageData{ L"", 0 };
		coverageData.AddModule(L"My Module").AddFile(L"My Directory/My File").AddLine(0, true);
		std::wostringstream ostr;
		Exporter::SonarQubeExporter().Export(coverageData, ostr);
		auto result = ostr.str();
		auto filename = L"\"My Directory/My File\"";
		ASSERT_TRUE(boost::algorithm::contains(result, filename));
	}
}