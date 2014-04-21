#include "stdafx.h"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "Tools/TemporaryFolder.hpp"

#include <boost/filesystem.hpp>

#include "Exporter/Html/HtmlExporter.hpp"
#include "Exporter/Html/HtmlFolderStructure.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace ExporterTest
{
	//-------------------------------------------------------------------------
	TEST(HtmlExporterTest, Export)
	{
		fs::path templateFolder = fs::canonical("../Exporter/Html/Template");
		fs::path testFolder = fs::canonical("../ExporterTest/Data");

		Exporter::HtmlExporter htmlExporter(templateFolder);
		cov::CoverageData data{ L"Test" };

		auto& module1 = data.AddModule(L"Module1.exe");
		auto& file1 = module1.AddFile(testFolder / L"TestFile1.cpp");
		auto& file2 = module1.AddFile(testFolder / L"TestFile2.cpp");

		data.AddModule(L"Module2.exe");

		Tools::TemporaryFolder output;
		htmlExporter.Export(data, output);

		auto modulesPath = output.GetPath() / Exporter::HtmlFolderStructure::FolderModules;
		ASSERT_TRUE(fs::exists(output.GetPath() / "index.html"));
		ASSERT_TRUE(fs::exists(modulesPath / "module1.html"));
		ASSERT_TRUE(fs::exists(modulesPath / "module2.html"));
		ASSERT_TRUE(fs::exists(modulesPath / "module1" / "TestFile1.html"));
		ASSERT_TRUE(fs::exists(modulesPath / "module1" / "TestFile2.html"));
	}
}

