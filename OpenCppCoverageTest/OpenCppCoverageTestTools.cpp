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

#include <boost/filesystem.hpp>
#include <Poco/Process.h>

#include "Tools/Tool.hpp"
#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/ProgramOptions.hpp"

#include "OpenCppCoverage/main.hpp"

namespace OpenCppCoverageTest
{
	//---------------------------------------------------------------------
	int RunCoverageOn(
		std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const boost::filesystem::path& outputFolder,
		const boost::filesystem::path& programToRun,
		const std::vector<std::wstring>& arguments)
	{		
		coverageArguments.push_back({ CppCoverage::ProgramOptions::OutputDirectoryOption, outputFolder.string() });		
		
		return RunCoverageOn(coverageArguments, programToRun, arguments);
	}

	//---------------------------------------------------------------------
	int RunCoverageOn(
		const std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const boost::filesystem::path& programToRun,
		const std::vector<std::wstring>& arguments)
	{
		std::vector<std::string> allCoverageArguments;
			
		for (const auto& keyValue : coverageArguments)
		{
			allCoverageArguments.push_back("--" + keyValue.first);
			allCoverageArguments.push_back(keyValue.second);
		}
				
		allCoverageArguments.push_back(programToRun.string());
		for (const auto& argument : arguments)
			allCoverageArguments.push_back(Tools::ToString(argument));

		boost::filesystem::path openCppCoverage = OpenCppCoverage::GetOutputBinaryPath();
		auto handle = Poco::Process::launch(openCppCoverage.string(), allCoverageArguments, ".", nullptr, nullptr, nullptr);
		int exitCode = handle.wait();

		return exitCode;
	}
}