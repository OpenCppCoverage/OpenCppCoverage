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
#include "OpenCppCoverageTestTools.hpp"

#include <filesystem>
#include <Poco/Process.h>
#include <Poco/Pipe.h>
#include <Poco/PipeStream.h>

#include "Tools/Tool.hpp"
#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/ProgramOptions.hpp"
#include "CppCoverage/ExportOptionParser.hpp"

namespace cov = CppCoverage;

namespace OpenCppCoverageTest
{
	//---------------------------------------------------------------------
	void AddDefaultFilters(
		std::vector<std::pair<std::string, std::string>>& coverageArguments,	
		const std::filesystem::path& programToRun)
	{
		coverageArguments.push_back({ cov::ProgramOptions::SelectedModulesOption, programToRun.string() });
		coverageArguments.push_back({ cov::ProgramOptions::SelectedSourcesOption, GetSolutionFolderName() });
	}

	//-------------------------------------------------------------------------
	void AddDefaultHtmlExport(
		std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const std::filesystem::path& outputFolder)
	{
		coverageArguments.push_back(
		{ cov::ExportOptionParser::ExportTypeOption, cov::ExportOptionParser::ExportTypeHtmlValue
		+ cov::ExportOptionParser::ExportSeparator + outputFolder.string() });		
	}

	//---------------------------------------------------------------------
	std::pair<std::string, std::string> BuildExportTypeString(
		const std::string& exportType,
		const std::filesystem::path& output)
	{
		return{
			cov::ExportOptionParser::ExportTypeOption,
			exportType + cov::ExportOptionParser::ExportSeparator + output.string() };
	}

	//-------------------------------------------------------------------------
	int RunCoverageFor(
		const std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const std::filesystem::path& programToRun,
		const std::vector<std::wstring>& arguments,
		std::string* optionalOutput)
	{
		std::vector<std::string> allCoverageArguments;

		for (const auto& keyValue : coverageArguments)
		{
			allCoverageArguments.push_back("--" + keyValue.first);
			const auto& value = keyValue.second;

			if (!value.empty())
				allCoverageArguments.push_back(value);
		}

		allCoverageArguments.push_back(programToRun.string());
		for (const auto& argument : arguments)
			allCoverageArguments.push_back(Tools::ToLocalString(argument));

		for (auto& argument : allCoverageArguments)
			argument = "\"" + argument + "\"";

		std::filesystem::path openCppCoverage(OUT_DIR);

		openCppCoverage /= "OpenCppCoverage.exe";

		auto pipe = optionalOutput ? std::make_unique<Poco::Pipe>() : nullptr;
		auto handle = Poco::Process::launch(openCppCoverage.string(),
		                                    allCoverageArguments,
		                                    ".",
		                                    nullptr,
		                                    pipe.get(),
		                                    pipe.get());
		if (pipe)
		{
			Poco::PipeInputStream istr{*pipe};
			*optionalOutput = {std::istreambuf_iterator<char>(istr),
			                   std::istreambuf_iterator<char>()};
		}

		int exitCode = handle.wait();

		return exitCode;
	}

	//-------------------------------------------------------------------------
	std::string GetSolutionFolderName()
	{
		return std::filesystem::canonical(SOLUTION_DIR).filename().string();
	}
}