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

#include <filesystem>
#include <fstream>

#include <boost/algorithm/string.hpp>

#include "Plugin/Exporter/CoverageData.hpp"
#include "Plugin/Exporter/ModuleCoverage.hpp"
#include "Plugin/Exporter/FileCoverage.hpp"

#include "Exporter/InvalidOutputFileException.hpp"
#include "Exporter/CoberturaExporter.hpp"
#include "tools/Tool.hpp"

#include "TestHelper/TemporaryPath.hpp"

namespace fs = std::filesystem;

namespace ExporterTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		std::wstring GetExpectedResult()
		{
			fs::path expectedResult = fs::path(PROJECT_DIR) / "Data" / "CoberturaExporterExpectedResult.xml";
			std::wifstream ifs{ expectedResult.wstring().c_str() };
			std::wostringstream ostr;

			ostr << ifs.rdbuf();

			return ostr.str();
		}
	}
	
	//-------------------------------------------------------------------------
	TEST(CoberturaExporterTest, Export)
	{
		Plugin::CoverageData coverageData{L"", 0};

		coverageData.AddModule(L"EmptyModule");
		auto& module = coverageData.AddModule(L"Module");

		module.AddFile("EmptyFile");
		auto& file = module.AddFile("File");

		file.AddLine(0, true);
		file.AddLine(1, false);

		module.AddFile("File2").AddLine(0, true);

		std::wostringstream ostr;
		Exporter::CoberturaExporter().Export(coverageData, ostr);
		auto result = ostr.str();
		auto expectedResult = GetExpectedResult();				
		
		ASSERT_EQ(result, expectedResult);
	}	

	//-------------------------------------------------------------------------
	TEST(CoberturaExporterTest, SubFolderDoesNotExist)
	{
		Plugin::CoverageData coverageData{ L"", 0 };
		TestHelper::TemporaryPath output;
		auto outputPath = output.GetPath() / "SubFolder" / "output.xml";

		ASSERT_FALSE(fs::exists(outputPath));
		Exporter::CoberturaExporter().Export(coverageData, outputPath);
		ASSERT_TRUE(fs::exists(outputPath));
	}

	//-------------------------------------------------------------------------
	TEST(CoberturaExporterTest, SpecialChars)
	{
		Plugin::CoverageData coverageData{ L"", 0 };
		coverageData.AddModule(L"��").AddFile(L"��").AddLine(0, true);
		
		std::wostringstream ostr;
		Exporter::CoberturaExporter().Export(coverageData, ostr);
		auto result = ostr.str();

		auto packageName = Tools::LocalToWString(u8"package name=\"��\"");
		auto name = Tools::LocalToWString(u8"class name=\"��\"");
		auto filename = Tools::LocalToWString(u8"filename=\"��\"");

		ASSERT_TRUE(boost::algorithm::contains(result, packageName));
		ASSERT_TRUE(boost::algorithm::contains(result, name));
		ASSERT_TRUE(boost::algorithm::contains(result, filename));
	}

	//-------------------------------------------------------------------------
	TEST(CoberturaExporterTest, OutputExists)
	{
		Plugin::CoverageData coverageData{ L"", 0 };
		TestHelper::TemporaryPath outputPath{ TestHelper::TemporaryPathOption::CreateAsFile };
		
		ASSERT_NO_THROW(Exporter::CoberturaExporter().Export(coverageData, outputPath));
	}

	//-------------------------------------------------------------------------
	TEST(CoberturaExporterTest, InvalidFile)
	{
		Plugin::CoverageData coverageData{L"", 0};
		TestHelper::TemporaryPath outputPath{
		    TestHelper::TemporaryPathOption::CreateAsFolder};

		ASSERT_THROW(Exporter::CoberturaExporter().Export(
		                 coverageData, outputPath.GetPath() / "InvalidFile/"),
		             Exporter::InvalidOutputFileException);
	}
}