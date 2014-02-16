#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "CppCoverage/FileCoverage.hpp"
#include "Exporter/Html/HtmlFileCoverageExporter.hpp"

namespace fs = boost::filesystem;

namespace ExporterTest
{
	TEST(HtmlFileCoverageExporterTest, Export)
	{
		fs::path path = "../ConsoleForCppCoverageTest/ConsoleForCppCoverageTest.cpp";
		
		CppCoverage::FileCoverage fileCoverage{path};
		Exporter::HtmlFileCoverageExporter exporter;
		std::wostringstream ostr;
		
		ASSERT_TRUE(fs::exists(path));

		fileCoverage.AddLine(8, true);
		fileCoverage.AddLine(9, true);
		fileCoverage.AddLine(10, false);

		exporter.Export(fileCoverage, ostr);
		
		std::vector<std::wstring> fields;
		boost::split(fields, ostr.str(), boost::is_any_of("\n"));
		
		ASSERT_LE(10u, fields.size());
		ASSERT_TRUE(boost::starts_with(fields[8], "OK"));
		ASSERT_TRUE(boost::starts_with(fields[9], "OK"));
		ASSERT_TRUE(boost::starts_with(fields[10], "KO"));
	}

}