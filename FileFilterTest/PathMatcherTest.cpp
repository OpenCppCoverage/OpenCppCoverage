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

#include <boost/optional.hpp>

#include "FileFilter/PathMatcher.hpp"
#include "FileFilter/File.hpp"
#include "FileFilter/AmbiguousPathException.hpp"
#include "Tools.hpp"

using namespace FileFilter;
namespace fs = boost::filesystem;

namespace FileFilterTest
{		
	namespace
	{
		//-------------------------------------------------------------------------
		std::wstring Match(PathMatcher& pathMatcher, const fs::path& path)
		{
			const auto* file = pathMatcher.Match(path);

			return (file) ? file->GetPath().wstring() : L"No file found";
		}
	}

	//-------------------------------------------------------------------------
	TEST(PathMatcherTest, PostFixBasicMatch)
	{
		std::vector<std::wstring> filenames = { L"Test.txt", L"Test\\Test2.txt" };
		auto files = ToFiles(filenames);
		PathMatcher pathMatcher{std::move(files), boost::none };

		ASSERT_EQ(filenames.at(0), Match(pathMatcher, "Test.txt"));
		ASSERT_EQ(filenames.at(1), Match(pathMatcher, "Test\\Test\\Test2.txt"));
		ASSERT_EQ(filenames.at(1), Match(pathMatcher, "test\\test\\test2.txt"));

		ASSERT_EQ(nullptr, pathMatcher.Match("Test2\\Test2.txt"));
		ASSERT_EQ(nullptr, pathMatcher.Match("Test2.txt"));
		ASSERT_EQ(nullptr, pathMatcher.Match("Test3.txt"));
		ASSERT_EQ(0, pathMatcher.GetUnmatchedPaths().size());
	}	

	//-------------------------------------------------------------------------
	TEST(PathMatcherTest, PostFixAmgigousPath)
	{
		std::vector<File> files;
		const fs::path filename{ L"Test.txt" };
		const fs::path test1{ L"Test1" / filename };
		const fs::path test2{ L"Test2" / filename };
		files.emplace_back(filename);
		
		PathMatcher pathMatcher{ std::move(files), boost::none };

		Match(pathMatcher, test1);
		
		try
		{
			Match(pathMatcher, test2);
			FAIL();
		}
		catch (const AmbiguousPathException& e)
		{
			ASSERT_EQ(filename, e.GetPostFixPath());
			ASSERT_EQ(test1, e.GetFirstPossiblePath());
			ASSERT_EQ(test2, e.GetSecondPossiblePath());
		}
	}
	
	//-------------------------------------------------------------------------
	TEST(PathMatcherTest, PostFixGetUnmatchedPaths)
	{		
		std::vector<std::wstring> filenames{ L"test1", L"test2", L"test\\test2", L"test3" };
		auto files = ToFiles(filenames);

		PathMatcher pathMatcher{ std::move(files), boost::none };

		pathMatcher.Match(filenames.at(1));
		auto unmatchedPaths = pathMatcher.GetUnmatchedPaths();
		ASSERT_EQ(3, unmatchedPaths.size());
		ASSERT_EQ(filenames.at(0), unmatchedPaths.at(0).wstring());
		ASSERT_EQ(filenames.at(2), unmatchedPaths.at(1).wstring());
		ASSERT_EQ(filenames.at(3), unmatchedPaths.at(2).wstring());
	}

	//-------------------------------------------------------------------------
	TEST(PathMatcherTest, FullPathBasicMatch)
	{
		std::vector<std::wstring> filenames{ L"Test.txt", L"Test2" };
		auto files = ToFiles(filenames);
		fs::path parentPath = "Test";
		PathMatcher pathMatcher{ std::move(files), parentPath };

		ASSERT_EQ(nullptr, pathMatcher.Match(filenames.at(0)));
		ASSERT_EQ(filenames.at(0), Match(pathMatcher, parentPath / filenames.at(0)));

		auto unmatchedPaths = pathMatcher.GetUnmatchedPaths();
		ASSERT_EQ(1, unmatchedPaths.size());
		ASSERT_EQ(filenames.at(1), unmatchedPaths.at(0));
	}
}