// CppCoverageConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/filesystem.hpp>

// $$ clean
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "Tools/Log.hpp"

#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/CoverageSettings.hpp"
#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Patterns.hpp"

#include "Exporter/Html/HtmlExporter.hpp"


#include "CppCoverage/CppCoverageLog.hpp"
namespace cov = CppCoverage;
namespace logging = boost::log;

int _tmain(int argc, _TCHAR* argv[])
{	
	auto filter = logging::trivial::severity >= logging::trivial::info;
	logging::core::get()->set_filter(filter); // $$ remove this code
		
	cov::SetLoggerMinSeverity(logging::trivial::info);

	if (argc != 3)
	{
		LOG_ERROR << L"Invalid number of argument" << std::endl; // $$ to clean
		return 1;
	}

	LOG_INFO << L"Start: " << std::wstring(argv[0]) << L" with arguments";

	for (int i = 0; i < argc; ++i)
		LOG_INFO << L'\t' << std::wstring(argv[i]);

	

	try
	{		
		cov::CodeCoverageRunner codeCoverageRunner;
		cov::StartInfo startInfo{ argv[1] };
		cov::Patterns modulePatterns{ false };
		cov::Patterns sourcePatterns{ false };

	//	startInfo.AddArguments(L"--gtest_filter=CoverageDataTest.*");
		cov::CoverageSettings settings{ modulePatterns, sourcePatterns };

		modulePatterns.AddPositivePatterns(L".*");
		sourcePatterns.AddPositivePatterns(L".*CppCoverage.*cpp"); // ConsoleForCppCoverageTest.cpp");

		//settings.AddSourcePositivePatterns(L"c:\\users\\olivier_2\\desktop\\dev\\cppcoverage");
	
		cov::CoverageData coverage = codeCoverageRunner.RunCoverage(startInfo, settings);
		fs::path templateFolder = fs::absolute("C:\\Users\\Olivier_2\\Desktop\\Dev\\CppCoverage\\Debug\\Template");
		Exporter::HtmlExporter htmlExporter{ templateFolder};

		boost::filesystem::path output = argv[2];
		htmlExporter.Export(coverage, output);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error:" << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown Error" << std::endl;
	}
	return 0;
}