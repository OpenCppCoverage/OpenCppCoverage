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

#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace OpenCppCoverageTest
{	
	void AddDefaultFilters(
		std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const std::filesystem::path& programToRun);

	void AddDefaultHtmlExport(
		std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const std::filesystem::path& outputFolder);
	
	std::pair<std::string, std::string> BuildExportTypeString(
		const std::string& exportType,
		const std::filesystem::path& output);
	
	int RunCoverageFor(
		const std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const std::filesystem::path& programToRun,
		const std::vector<std::wstring>& arguments,
		std::string* optionalOutput = nullptr);

	std::string GetSolutionFolderName();
}