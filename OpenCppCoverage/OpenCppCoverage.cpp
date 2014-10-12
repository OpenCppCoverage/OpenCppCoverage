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
#include "OpenCppCoverage.hpp"

namespace OpenCppCoverage
{
}

#include "stdafx.h"
#include "OpenCppCoverage.hpp"

#include <iomanip>

#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/CoverageSettings.hpp"
#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/Options.hpp"
#include "CppCoverage/ProgramOptions.hpp"

#include "Exporter/Html/HtmlExporter.hpp"
#include "Exporter/CoberturaExporter.hpp"

#include "Tools/Tool.hpp"
#include "Tools/Log.hpp"

namespace cov = CppCoverage;
namespace logging = boost::log;

namespace OpenCppCoverage
{
	
	namespace
	{
		//-----------------------------------------------------------------------------
		fs::path GetHtmlOutputPath(const boost::optional<fs::path>& outputPath)
		{
			if (outputPath)
				return *outputPath;

			auto now = std::time(nullptr);
			auto localNow = std::localtime(&now);
			std::ostringstream ostr;

			ostr << "CoverageReport-" << std::put_time(localNow, "%Y-%m-%d-%Hh%Mm%Ss");

			return ostr.str();
		}

		//-----------------------------------------------------------------------------
		fs::path GetCoberturaOutput(
			const cov::Options& options,
			const boost::optional<fs::path>& outputPath)
		{
			if (outputPath)
				return *outputPath;

			auto path = options.GetStartInfo().GetPath();

			path = path.filename().replace_extension("");
			path += "Coverage.xml";
			
			return path;
		}

		//-----------------------------------------------------------------------------
		void ExportHtml(
			const cov::Options& options,
			const cov::CoverageData& coverage,
			const boost::optional<fs::path>& outputPath)
		{
			fs::path templateFolder = Tools::GetTemplateFolder();
			Exporter::HtmlExporter htmlExporter{ templateFolder };

			boost::filesystem::path output = GetHtmlOutputPath(outputPath);
			htmlExporter.Export(coverage, output);			
		}

		//-----------------------------------------------------------------------------
		void ExportCovertura(
			const cov::Options& options, 
			const cov::CoverageData& coverage,
			const boost::optional<fs::path>& outputPath)
		{
			Exporter::CoberturaExporter coberturaExporter;
			auto coverageOutput = GetCoberturaOutput(options, outputPath);
			coberturaExporter.Export(coverage, coverageOutput);
		}

		//-----------------------------------------------------------------------------
		void Export(
			const cov::Options& options, 
			const cov::CoverageData& coverage)
		{
			const auto& exports = options.GetExports();
			const static std::map<
				cov::OptionsExportType, 
				std::function<void(
						const cov::Options&, 
						const cov::CoverageData&, 
						const boost::optional<fs::path>&) >> exporters =
			{
				{ cov::OptionsExportType::Html, ExportHtml },
				{ cov::OptionsExportType::Cobertura, ExportCovertura },
			};

			for (const auto& singleExport : exports)
			{
				const auto& exporter = exporters.at(singleExport.GetType());

				exporter(options, coverage, singleExport.GetOutputPath());
			}
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
			Export(options, coverage);

			auto exitCode = coverage.GetExitCode();

			if (exitCode)
				LOG_ERROR << L"Your program stop with error code: " << exitCode;
			return exitCode;
		}
	}

	//-----------------------------------------------------------------------------
	int OpenCppCoverage::Run(int argc,
		const char** argv,
		std::wostream* emptyOptionsExplanation) const
	{
		cov::OptionsParser optionsParser;

		auto options = optionsParser.Parse(argc, argv, emptyOptionsExplanation);

		if (options)
			return ::OpenCppCoverage::Run(*options);
		return 1;
	}
}