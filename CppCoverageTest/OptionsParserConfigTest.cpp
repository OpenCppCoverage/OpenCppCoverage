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

#include <boost/optional.hpp>

#include "CppCoverage/ProgramOptions.hpp"
#include "CppCoverage/Options.hpp"
#include "CppCoverage/OptionsParser.hpp"

#include "CppCoverageTest/TestTools.hpp"

#include "Tools/TemporaryPath.hpp"
#include "Tools/Tool.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		//-------------------------------------------------------------------------	
		boost::optional<cov::Options> MutipleSourceParse(
			const std::vector<std::pair<std::string, std::wstring>>& configArguments,
			const std::vector<std::pair<std::string, std::wstring>>& commandLineArguments)
		{
			cov::OptionsParser parser;
			Tools::TemporaryPath path;
			std::wofstream ofs(path.GetPath().string().c_str());

			for (const auto& argument : configArguments)
				ofs << Tools::ToWString(argument.first) << L"=" << argument.second << std::endl;

			std::vector<std::string> arguments{ "--" + cov::ProgramOptions::ConfigFileOption, path.GetPath().string() };

			for (const auto& argumentValue : commandLineArguments)
			{
				arguments.push_back(TestTools::OptionPrefix + argumentValue.first);
				arguments.push_back(Tools::ToString(argumentValue.second));
			}

			return TestTools::Parse(parser, arguments);
		}
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserConfigTest, ConfigurationFileMultipleValue)
	{
		const std::wstring source1 = L"s1";
		const std::wstring source2 = L"s2";

		auto options = MutipleSourceParse(
		{ { cov::ProgramOptions::SelectedSourcesOption, source1 }, { cov::ProgramOptions::SelectedSourcesOption, source2 } },
		{});
		ASSERT_TRUE(options);
		const auto& sourcePatterns = options->GetSourcePatterns().GetSelectedPatterns();
		ASSERT_EQ(2, sourcePatterns.size());
		ASSERT_EQ(source1, sourcePatterns[0]);
		ASSERT_EQ(source2, sourcePatterns[1]);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserConfigTest, ConfigurationFileCmdLineOverride)
	{
		const std::wstring source1 = L"s1";
		const std::wstring source2 = L"s2";

		auto options = MutipleSourceParse(
		{ { cov::ProgramOptions::OutputDirectoryOption, source1 } },
		{ { cov::ProgramOptions::OutputDirectoryOption, source2 } });
		ASSERT_TRUE(options);
		auto outputDirectory = options->GetOutputDirectoryOption();
		ASSERT_TRUE(outputDirectory);
		ASSERT_EQ(source2, outputDirectory->wstring());
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserConfigTest, ConfigurationFileMerge)
	{
		const std::wstring source1 = L"s1";
		const std::wstring source2 = L"s2";

		auto options = MutipleSourceParse(
		{ { cov::ProgramOptions::SelectedSourcesOption, source1 } },
		{ { cov::ProgramOptions::SelectedSourcesOption, source2 } });
		ASSERT_TRUE(options);
		const auto& sourcePatterns = options->GetSourcePatterns().GetSelectedPatterns();
		ASSERT_EQ(2, sourcePatterns.size());
		ASSERT_EQ(source2, sourcePatterns[0]);
		ASSERT_EQ(source1, sourcePatterns[1]);
	}

	//-------------------------------------------------------------------------
	TEST(OptionsParserConfigTest, ConfigurationFileInvalidOption)
	{
		auto options = MutipleSourceParse({ { cov::ProgramOptions::ConfigFileOption, L"." } }, {});

		ASSERT_FALSE(options);
	}
}