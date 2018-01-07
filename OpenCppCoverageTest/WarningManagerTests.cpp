// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2018 OpenCppCoverage
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
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "OpenCppCoverageTestTools.hpp"
#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "CppCoverage/ProgramOptions.hpp"
#include "CppCoverage/OptionsParser.hpp"
#include "Tools/Tool.hpp"

using CppCoverage::OptionsParser;
using CppCoverage::ProgramOptions;

namespace OpenCppCoverageTest
{
	//-------------------------------------------------------------------------
	TEST(WarningManagerTests, CommandLineTooLong)
	{
		std::string stdOut;
		const std::vector<std::pair<std::string, std::string>>
		    coverageArguments = {
		        {ProgramOptions::SelectedSourcesOption,
		         TestCoverageConsole::GetMainCppPath().string()},
		        {ProgramOptions::SelectedModulesOption,
		         TestCoverageConsole::GetOutputBinaryPath().string()},
		        {ProgramOptions::QuietOption, ""}};
		std::vector<std::wstring> arguments{TestCoverageConsole::TestBasic};

		RunCoverageFor(coverageArguments,
		               TestCoverageConsole::GetOutputBinaryPath(),
		               arguments,
		               &stdOut);
		ASSERT_EQ("", stdOut);
		for (int i = 0; i < OptionsParser::DosCommandLineMaxSize / 10; ++i)
			arguments.push_back(L"01223456789");

		RunCoverageFor(coverageArguments,
		               TestCoverageConsole::GetOutputBinaryPath(),
		               arguments,
		               &stdOut);
		auto output = Tools::LocalToWString(stdOut);
		ASSERT_TRUE(boost::algorithm::contains(
		    output, OptionsParser::GetTooLongCommandLineMessage()));
	}
}
