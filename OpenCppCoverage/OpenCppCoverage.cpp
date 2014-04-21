// CppCoverageConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/log/trivial.hpp>

#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/Options.hpp"
#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/CoverageSettings.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/StartInfo.hpp"

#include "Exporter/Html/HtmlExporter.hpp"

#include "Tools/Tool.hpp"
#include "Tools/Log.hpp"

#include "CppCoverageConsoleException.hpp"

namespace cov = CppCoverage;
namespace logging = boost::log;

namespace
{
	//-----------------------------------------------------------------------------
	fs::path GetExecutablePath()
	{
		std::vector<wchar_t> filename(4096);

		if (!GetModuleFileName(nullptr, &filename[0], filename.size()))
			THROW("Cannot get current executable path.");

		return fs::path{&filename[0]};
	}

	//-----------------------------------------------------------------------------
	fs::path GetExecutableFolder()
	{
		fs::path executablePath = GetExecutablePath();

		return executablePath.parent_path();
	}

	// $$ remove warning compilations
	//-----------------------------------------------------------------------------
	void Run(const cov::Options& options)
	{		
		auto logLevel = (options.IsVerboseModeSelected()) ? logging::trivial::debug : logging::trivial::info;
		Tools::InitConsoleAndFileLog(L"Test.log"); // $$ todo
		Tools::SetLoggerMinSeverity(logLevel);

		const auto& startInfo = options.GetStartInfo();
		// $$ check if code run correctly add log + export !!!
		cov::CodeCoverageRunner codeCoverageRunner;
		cov::CoverageSettings settings{ options.GetModulePatterns(), options.GetSourcePatterns() };

		std::wostringstream ostr; //$$ redirect executable code output?
		ostr << std::endl << options;
		LOG_INFO << L"Start Program:" << ostr.str();

		cov::CoverageData coverage = codeCoverageRunner.RunCoverage(startInfo, settings);
		fs::path templateFolder = GetExecutableFolder() / "Template";
		Exporter::HtmlExporter htmlExporter{ templateFolder };
		
		boost::filesystem::path output = "Coverage"; // $$ add the name of the module
		htmlExporter.Export(coverage, output);
	}
}

//-----------------------------------------------------------------------------
int main(int argc, const char* argv[])
{	
	cov::OptionsParser optionsParser;

	try
	{		
		auto options = optionsParser.Parse(argc, argv);

		if (options)
		{
			Run(*options);
			return 0;
		}
		
		std::wcerr << optionsParser << std::endl;
	}
	catch (const boost::program_options::unknown_option& unknownOption)
	{
		std::wcerr << unknownOption.what() << std::endl;
		std::wcerr << optionsParser << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error:" << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown Error:" << std::endl;
	}

	return 1;
}