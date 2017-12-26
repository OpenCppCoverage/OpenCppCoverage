// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage
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
#include "CppCoverage/ProgramOptions.hpp"
#include "CppCoverage/UnifiedDiffSettings.hpp"

#include "CppCoverageTest/TestTools.hpp"
#include "TestHelper/TemporaryPath.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace CppCoverageTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		boost::optional<cov::Options> Parse(
			const std::vector<cov::UnifiedDiffSettings>& unifiedDiffSettingsCollection)
		{
			cov::OptionsParser parser;
			std::wostringstream ostr;
			std::vector<std::string> arguments;

			for (const auto& unifiedDiffSettings : unifiedDiffSettingsCollection)
			{
				arguments.push_back(TestTools::GetOptionPrefix() + cov::ProgramOptions::UnifiedDiffOption);

				auto value = unifiedDiffSettings.GetUnifiedDiffPath().string();
				auto rootDiffFolder = unifiedDiffSettings.GetRootDiffFolder();
				if (rootDiffFolder)
					value += cov::OptionsParser::PathSeparator + rootDiffFolder->string();
				arguments.push_back(value);
			}

			auto option = TestTools::Parse(parser, arguments, true, &ostr);

			if (!option && ostr.str().empty())
				throw std::runtime_error("Expect error message.");
			return option;
		}


		//-------------------------------------------------------------------------
		std::vector<cov::UnifiedDiffSettings> ToVector(cov::UnifiedDiffSettings&& unifiedDiffSettings)
		{
			std::vector<cov::UnifiedDiffSettings> unifiedDiffSettingsCollection;

			unifiedDiffSettingsCollection.push_back(std::move(unifiedDiffSettings));
			return unifiedDiffSettingsCollection;
		}

		//-------------------------------------------------------------------------
		boost::optional<cov::Options> Parse(cov::UnifiedDiffSettings&& unifiedDiffSettings)
		{
			return Parse(ToVector(std::move(unifiedDiffSettings)));
		}

		//---------------------------------------------------------------------
		bool AreEqual(
			const std::vector<cov::UnifiedDiffSettings>& settings1,
			const std::vector<cov::UnifiedDiffSettings>& settings2)
		{
			return std::equal(settings1.begin(), settings1.end(), settings2.begin(), settings2.end(),
				[](const auto& setting1, const auto& setting2)
			{
				return setting1.GetUnifiedDiffPath() == setting2.GetUnifiedDiffPath()
					&& setting1.GetRootDiffFolder() == setting2.GetRootDiffFolder();
			});
		}

		//---------------------------------------------------------------------
		void CheckUnifiedDiffSettings(const std::vector<cov::UnifiedDiffSettings>& unifiedDiffSettingsCollection)
		{
			auto option = Parse(unifiedDiffSettingsCollection);

			ASSERT_TRUE(!!option);
			ASSERT_TRUE(AreEqual(unifiedDiffSettingsCollection, option->GetUnifiedDiffSettingsCollection()));
		}

		//---------------------------------------------------------------------
		void CheckUnifiedDiffSettings(cov::UnifiedDiffSettings&& unifiedDiffSettings)
		{
			CheckUnifiedDiffSettings(ToVector(std::move(unifiedDiffSettings)));
		}

		//---------------------------------------------------------------------
		struct OptionsParserUnifiedDifftTest : public testing::Test
		{
			TestHelper::TemporaryPath temporaryPath{ TestHelper::TemporaryPathOption::CreateAsFile };
		};
	}

	//-------------------------------------------------------------------------
	TEST_F(OptionsParserUnifiedDifftTest, UnifiedDiffPath)
	{
		CheckUnifiedDiffSettings({ temporaryPath, boost::none });
	}

	//-------------------------------------------------------------------------
	TEST_F(OptionsParserUnifiedDifftTest, NotFoundUnifiedDiffPath)
	{
		ASSERT_TRUE(!Parse({ "Unknow", boost::none }));
	}

	//-------------------------------------------------------------------------
	TEST_F(OptionsParserUnifiedDifftTest, RootDiffFolder)
	{
		CheckUnifiedDiffSettings({ temporaryPath, fs::current_path() });
	}

	//-------------------------------------------------------------------------
	TEST_F(OptionsParserUnifiedDifftTest, NotFoundRootDiffFolder)
	{
		ASSERT_TRUE(!!Parse({ temporaryPath, boost::none }));
		ASSERT_FALSE(!!Parse({ temporaryPath, fs::path("Unknow") }));
	}

	//-------------------------------------------------------------------------
	TEST_F(OptionsParserUnifiedDifftTest, TwoDiff)
	{
		std::vector<cov::UnifiedDiffSettings> unifiedDiffSettingsCollection;
		unifiedDiffSettingsCollection.emplace_back(temporaryPath, fs::current_path());
		unifiedDiffSettingsCollection.emplace_back(temporaryPath, boost::none);

		CheckUnifiedDiffSettings(unifiedDiffSettingsCollection);
	}
}