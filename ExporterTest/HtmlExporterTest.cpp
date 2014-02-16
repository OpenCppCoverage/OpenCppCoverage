#include "stdafx.h"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"

#include <boost/filesystem.hpp>

#include "Exporter/Html/HtmlExporter.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace ExporterTest
{
	TEST(HtmlExporterTest, Export)
	{
		fs::path templateFolder = fs::canonical("../Exporter/Html/Template");
		fs::path testFolder = fs::canonical("../ExporterTest/Data");

		Exporter::HtmlExporter htmlExporter(templateFolder / "IndexTemplate.html", templateFolder / "ModuleTemplate.html");
		cov::CoverageData data{ L"Test" };

		auto& module1 = data.AddModule(L"Module1.exe");
		auto& file1 = module1.AddFile(testFolder / L"TestFile1.cpp");
		auto& file2 = module1.AddFile(testFolder / L"TestFile2.cpp");
		
		data.AddModule(L"Module2.exe");
		fs::path output = fs::absolute(fs::unique_path());
		fs::create_directory(output);
		try
		{
			htmlExporter.Export(data, output);
			ASSERT_TRUE(fs::exists(output / "index.html"));
			ASSERT_TRUE(fs::exists(output / "module1.html"));
			ASSERT_TRUE(fs::exists(output / "module2.html"));
			ASSERT_TRUE(fs::exists(output / "module1" / "TestFile1.html"));
			ASSERT_TRUE(fs::exists(output / "module1" / "TestFile2.html"));
		}
		catch (...)
		{
			fs::remove_all(output);
			throw;
		}
		fs::remove_all(output);
	}

}