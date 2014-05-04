// CppCoverageConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OpenCppCoverage.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>

#include <boost/log/trivial.hpp>

#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/CoverageSettings.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/Options.hpp"

#include "Exporter/Html/HtmlExporter.hpp"

#include "Tools/Tool.hpp"
#include "Tools/Log.hpp"

#include "OpenCppCoverageException.hpp"

namespace cov = CppCoverage;
namespace logging = boost::log;

namespace OpenCppCoverage
{
	//-------------------------------------------------------------------------
	boost::filesystem::path GetOutputBinaryPath()
	{
		return TARGET_PATH;
	}

	namespace
	{
		//-----------------------------------------------------------------------------
		fs::path GetExecutablePath()
		{
			std::vector<wchar_t> filename(4096);

			if (!GetModuleFileName(nullptr, &filename[0], filename.size()))
				THROW("Cannot get current executable path.");

			return fs::path{ &filename[0] };
		}

		//-----------------------------------------------------------------------------
		fs::path GetExecutableFolder()
		{
			fs::path executablePath = GetExecutablePath();

			return executablePath.parent_path();
		}

		//-----------------------------------------------------------------------------
		fs::path GetOutputPath()
		{
			auto now = std::time(nullptr);
			auto localNow = std::localtime(&now);
			std::ostringstream ostr;

			ostr << "CoverageReport-" << std::put_time(localNow, "%Y-%m-%d-%Hh%Mm%Ss");

			return ostr.str();
		}

		//-----------------------------------------------------------------------------
		int Run(const cov::Options& options)
		{
			auto logLevel = (options.IsVerboseModeSelected()) ? logging::trivial::debug : logging::trivial::info;
			Tools::InitConsoleAndFileLog(L"LastCoverageResults.log");
			Tools::SetLoggerMinSeverity(logLevel);

			const auto& startInfo = options.GetStartInfo();
			cov::CodeCoverageRunner codeCoverageRunner;
			cov::CoverageSettings settings{ options.GetModulePatterns(), options.GetSourcePatterns() };

			std::wostringstream ostr;
			ostr << std::endl << options;
			LOG_INFO << L"Start Program:" << ostr.str();

			cov::CoverageData coverage = codeCoverageRunner.RunCoverage(startInfo, settings);
			fs::path templateFolder = GetExecutableFolder() / "Template";
			Exporter::HtmlExporter htmlExporter{ templateFolder };

			auto now = std::time(nullptr);
			auto localNow = std::localtime(&now);

			boost::filesystem::path output = GetOutputPath();
			htmlExporter.Export(coverage, output);
			return coverage.GetExitCode();
		}
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
			return OpenCppCoverage::Run(*options);			
		
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