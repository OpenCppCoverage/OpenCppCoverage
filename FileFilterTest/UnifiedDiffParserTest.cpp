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

#include "FileFilter/UnifiedDiffParser.hpp"
#include "FileFilter/File.hpp"
#include "FileFilter/UnifiedDiffParserException.hpp"

#include <boost/filesystem.hpp>

#include "Tools/Tool.hpp"

using namespace FileFilter;

namespace FileFilterTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		const File& GetFile(
			const std::vector<File>& files,
			const boost::filesystem::path& path)
		{
			auto it = std::find_if(files.begin(), files.end(),
				[&](const File& file) { return file.GetPath() == path; });
			if (it == files.end())
				throw std::runtime_error("Cannot find: " + path.string());

			return *it;
		}

		//-------------------------------------------------------------------------
		int GetSelectedLinesCount(
			const std::vector<File>& files, 
			const boost::filesystem::path& path)
		{
			const auto& file = GetFile(files, path);
			return file.GetSelectedLines().size();
		}		

		//---------------------------------------------------------------------
		struct UnifiedDiffParserTest : public testing::Test
		{
			//-------------------------------------------------------------------------
			std::wstring GetError(const std::wstring& diffContent) const
			{
				std::wistringstream istr{ diffContent };
				try
				{
					unifiedDiffParser_.Parse(istr);
				}
				catch (UnifiedDiffParserException& e)
				{
					auto fullMessage = Tools::ToWString(e.what());
					auto index = fullMessage.find('\n');

					if (index != std::string::npos)
						return fullMessage.substr(index + 1);
				}
				return L"";
			}

			UnifiedDiffParser unifiedDiffParser_;
			const boost::filesystem::path diffPath_{ boost::filesystem::path(PROJECT_DIR)/ "Data" / "test.diff" };
		};
	}
		
	//-------------------------------------------------------------------------
	TEST_F(UnifiedDiffParserTest, ExpectedFiles)
	{
		std::wifstream diffFile{ diffPath_.string() };
		auto files = unifiedDiffParser_.Parse(diffFile);
		
		ASSERT_EQ(1, GetSelectedLinesCount(files, "b/CppCoverage/CodeCoverageRunner.cpp"));
		ASSERT_EQ(3, GetSelectedLinesCount(files, "b/CppCoverage/CoverageData.cpp"));
		ASSERT_EQ(2, GetSelectedLinesCount(files, "b/CppCoverage/CoverageData.hpp"));
		ASSERT_EQ(14, GetSelectedLinesCount(files, "b/CppCoverage/ExecutedAddressManager.cpp"));
		ASSERT_EQ(6, GetSelectedLinesCount(files, "b/CppCoverage/ExecutedAddressManager.hpp"));
		ASSERT_EQ(4, GetSelectedLinesCount(files, "b/CppCoverageTest/CodeCoverageRunnerTest.cpp"));
		ASSERT_EQ(4, GetSelectedLinesCount(files, "b/CppCoverageTest/ExecutedAddressManagerTest.cpp"));
		ASSERT_EQ(1, GetSelectedLinesCount(files, "b/PropertySheets/Default.props"));
		ASSERT_EQ(0, GetSelectedLinesCount(files, "b/TestHelper/Container.hpp"));
		ASSERT_EQ(0, GetSelectedLinesCount(files, "b/TestHelper/Container.inl"));
		ASSERT_EQ(0, GetSelectedLinesCount(files, "b/TestHelper/CoverageDataComparer.cpp"));
		ASSERT_EQ(0, GetSelectedLinesCount(files, "b/TestHelper/CoverageDataComparer.hpp"));
	}

	//-------------------------------------------------------------------------
	TEST_F(UnifiedDiffParserTest, ExpectedUpdatedLines)
	{
		std::wifstream diffFile{ diffPath_.string() };
		auto files = unifiedDiffParser_.Parse(diffFile);

		const auto& file = GetFile(files, "b/CppCoverage/ExecutedAddressManager.cpp");
		const auto& lines = file.GetSelectedLines();
		const std::set<int> expectedLines =
				{93, 95, 105, 130, 132, 133, 134, 154, 155, 156, 158, 159, 160, 162};

		ASSERT_EQ(expectedLines, lines);
	}
	
	//-------------------------------------------------------------------------
	TEST_F(UnifiedDiffParserTest, ErrorNoFilenameBeforeHunks)
	{
		ASSERT_EQ(UnifiedDiffParserException::ErrorNoFilenameBeforeHunks,
			GetError(L"@@"));
	}
	
	//-------------------------------------------------------------------------
	TEST_F(UnifiedDiffParserTest, ErrorCannotReadLine)
	{
		ASSERT_EQ(UnifiedDiffParserException::ErrorCannotReadLine,
			GetError(L"---"));
	}
	
	//-------------------------------------------------------------------------
	TEST_F(UnifiedDiffParserTest, ErrorExpectFromFilePrefix)
	{
		ASSERT_EQ(UnifiedDiffParserException::ErrorExpectFromFilePrefix,
			GetError(L"---\nXXX"));
	}
	
	//-------------------------------------------------------------------------
	TEST_F(UnifiedDiffParserTest, ErrorInvalidHunks)
	{
		ASSERT_EQ(UnifiedDiffParserException::ErrorInvalidHunks,
			GetError(L"---\n+++ \n@@ -1,2 +3,X"));
	}

	//-------------------------------------------------------------------------
	TEST_F(UnifiedDiffParserTest, ErrorContextHunks)
	{
		ASSERT_EQ(UnifiedDiffParserException::ErrorContextHunks,
			GetError(L"---\n+++ \n@@ -1,2 +3,4\n"));
	}

	//-------------------------------------------------------------------------
	TEST_F(UnifiedDiffParserTest, FromFileModificationTime)
	{
		std::wistringstream istr{ L"--- foo\t2002-02-21 23:30:39.942229878 -0800\n"
								  L"+++ bar\t2002-02 - 21 23:30 : 50.442260588 - 0800\n"
								  L"@@ -1,0 +1,0 @@" };
		auto files = unifiedDiffParser_.Parse(istr);

		ASSERT_EQ(1, files.size());
		ASSERT_EQ(L"bar", files.at(0).GetPath().wstring());
	}
}