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
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "TestHelper/TemporaryPath.hpp"

#include <boost/filesystem.hpp>

#include "Exporter/Html/HtmlExporter.hpp"
#include "Exporter/Html/HtmlFolderStructure.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace ExporterTest
{
	//-------------------------------------------------------------------------
	struct HtmlExporterTest : public ::testing::Test
	{
		//-------------------------------------------------------------------------
		HtmlExporterTest()
			: htmlExporter_{ fs::canonical(OUT_DIR) / "Template" }
		{

		}

		//---------------------------------------------------------------------
		bool Contains(std::wistream& istr, const std::wstring& str)
		{			
			std::wstring line;

			while (std::getline(istr, line))
			{
				if (line.find(str) != std::string::npos)
					return true;
			}

			return false;
		}

		//---------------------------------------------------------------------
		void CheckWarningInIndex(bool expectedValue)
		{
			auto indexPath = output_.GetPath() / "index.html";
			ASSERT_TRUE(fs::exists(indexPath));
			std::wifstream ifs{ indexPath.string()};
			bool hasWarning = Contains(ifs, Exporter::HtmlExporter::WarningExitCodeMessage);
			
			ASSERT_EQ(expectedValue, hasWarning);
		}

		Exporter::HtmlExporter htmlExporter_;
		TestHelper::TemporaryPath output_;
	};	

	//-------------------------------------------------------------------------
	TEST_F(HtmlExporterTest, Export)
	{	
		fs::path testFolder = fs::path(PROJECT_DIR) / "Data";
		cov::CoverageData data{ L"Test", 0};
		std::wstring filename1{ L"TestFile1.cpp"};
		std::wstring filename2{ L"TestFile2.cpp"};

		auto& module1 = data.AddModule(L"Module1.exe");
		auto& file1 = module1.AddFile(testFolder / filename1);
		auto& file2 = module1.AddFile(testFolder / filename2);
		
		file1.AddLine(0, true);
		file2.AddLine(0, true);
		
		data.AddModule(L"Module2.exe");		

		htmlExporter_.Export(data, output_);

		auto modulesPath = output_.GetPath() / Exporter::HtmlFolderStructure::FolderModules;
		ASSERT_TRUE(fs::exists(output_.GetPath() / "index.html"));
		ASSERT_TRUE(fs::exists(modulesPath / "module1.html"));
		ASSERT_FALSE(fs::exists(modulesPath / "module2.html"));
		ASSERT_TRUE(fs::exists(modulesPath / "module1" / (filename1 + L".html")));
		ASSERT_TRUE(fs::exists(modulesPath / "module1" / (filename2 + L".html")));
	}

	//-------------------------------------------------------------------------
	TEST_F(HtmlExporterTest, NoWarning)
	{
		cov::CoverageData data{ L"Test", 0 };

		htmlExporter_.Export(data, output_);		
		CheckWarningInIndex(false);
	}

	//-------------------------------------------------------------------------
	TEST_F(HtmlExporterTest, Warning)
	{
		cov::CoverageData data{ L"Test", 42};

		htmlExporter_.Export(data, output_);	
		CheckWarningInIndex(true);
	}

	//-------------------------------------------------------------------------
	TEST_F(HtmlExporterTest, SubFolderDoesNotExist)
	{
		cov::CoverageData data{ L"Test", 42 };
		auto outputFolder = output_.GetPath() / "SubFolder1" / "SubFolder2";

		ASSERT_FALSE(fs::exists(outputFolder));
		htmlExporter_.Export(data, outputFolder);
		ASSERT_TRUE(fs::exists(outputFolder));
	}
}

