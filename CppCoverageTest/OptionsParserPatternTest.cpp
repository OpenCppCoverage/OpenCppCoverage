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

#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/Options.hpp"
#include "CppCoverage/ProgramOptions.hpp"

#include "CppCoverageTest/TestTools.hpp"

#include "Tools/Tool.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		void CheckPatternOption(
			const std::string& optionName,
			const std::wstring& value,
			std::function<std::wstring(const cov::Options&)> getOption)
		{
			cov::OptionsParser parser;
			std::vector<std::string> arguments = { TestTools::OptionPrefix + optionName, 
													Tools::ToLocalString(value) };

			auto options = TestTools::Parse(parser, arguments);
			auto option = getOption(*options);

			ASSERT_EQ(value, option);
		}
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserPatternTest, SelectedModulePatterns)
	{
		CheckPatternOption(cov::ProgramOptions::SelectedModulesOption, L"module",
			[](const cov::Options& options) { return options.GetModulePatterns().GetSelectedPatterns().front(); }
		);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserPatternTest, ExcludedModulePatterns)
	{
		CheckPatternOption(cov::ProgramOptions::ExcludedModulesOption, L"module",
			[](const cov::Options& options) { return options.GetModulePatterns().GetExcludedPatterns().front(); }
		);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserPatternTest, SelectedSourcePatterns)
	{
		CheckPatternOption(cov::ProgramOptions::SelectedSourcesOption, L"source",
			[](const cov::Options& options) { return options.GetSourcePatterns().GetSelectedPatterns().front(); }
		);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserPatternTest, ExcludedSourcePatterns)
	{
		CheckPatternOption(cov::ProgramOptions::ExcludedSourcesOption, L"source",
			[](const cov::Options& options) { return options.GetSourcePatterns().GetExcludedPatterns().front(); }
		);
	}
}