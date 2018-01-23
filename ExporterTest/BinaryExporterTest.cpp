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

#include "CppCoverage/CoverageData.hpp"
#include "Exporter/Binary/BinaryExporter.hpp"
#include "TestHelper/TemporaryPath.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace ExporterTest
{
	//-------------------------------------------------------------------------
	TEST(BinaryExporterTest, SubFolderDoesNotExist)
	{
		cov::CoverageData coverageData{ L"", 0 };

		TestHelper::TemporaryPath output;
		auto outputPath = output.GetPath() / "SubFolder" / "output.cov";

		ASSERT_FALSE(fs::exists(outputPath));
		Exporter::BinaryExporter().Export(coverageData, outputPath);
		ASSERT_TRUE(fs::exists(outputPath));
	}

	//-------------------------------------------------------------------------
	TEST(BinaryExporterTest, OutputExists)
	{
		cov::CoverageData coverageData{ L"", 0 };

		TestHelper::TemporaryPath outputPath{ TestHelper::TemporaryPathOption::CreateAsFile };
		
		ASSERT_NO_THROW(Exporter::BinaryExporter().Export(coverageData, outputPath.GetPath()));
	}

	//-------------------------------------------------------------------------
	TEST(BinaryExporterTest, InvalidFile)
	{
		cov::CoverageData coverageData{L"", 0};

		TestHelper::TemporaryPath outputPath{
		    TestHelper::TemporaryPathOption::CreateAsFolder};

		ASSERT_THROW(Exporter::BinaryExporter().Export(
		                 coverageData, outputPath.GetPath() / "InvalidFile/"),
		             std::exception);
	}
}