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

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "FileFilter/UnifiedDiffCoverageFilter.hpp"
#include "FileFilter/File.hpp"
#include "Tools.hpp"

using namespace FileFilter;

namespace FileFilterTest
{
	namespace
	{			
		const std::vector<std::wstring> fileNames_{ L"file1", L"file2" };			
	}

	//-------------------------------------------------------------------------
	TEST(UnifiedDiffCoverageFilterTest, IsSourceFileSelected)
	{			
		UnifiedDiffCoverageFilter filter{ ToFiles(fileNames_), boost::none };

		ASSERT_TRUE(filter.IsSourceFileSelected(fileNames_.at(0)));
		ASSERT_TRUE(filter.IsSourceFileSelected(fileNames_.at(0)));
		ASSERT_FALSE(filter.IsSourceFileSelected(L"Unknow"));
		ASSERT_TRUE(filter.IsSourceFileSelected(fileNames_.at(1)));
	}

	//-------------------------------------------------------------------------
	TEST(UnifiedDiffCoverageFilterTest, IsLineSelected)
	{
		auto files = ToFiles(fileNames_);
		const auto line = 42;

		files.at(0).AddSelectedLines({ line });
		UnifiedDiffCoverageFilter filter{std::move(files), boost::none };

		ASSERT_TRUE(filter.IsLineSelected(fileNames_.at(0), line));
		ASSERT_FALSE(filter.IsLineSelected(fileNames_.at(0), line + 1));
		ASSERT_FALSE(filter.IsLineSelected(fileNames_.at(1), line));
		ASSERT_FALSE(filter.IsLineSelected("Unknow", line));
	}

	//-------------------------------------------------------------------------
	TEST(UnifiedDiffCoverageFilterTest, AmbiguousPathException)
	{
		std::vector<std::wstring> filenames = { L"file1" };
		auto files = ToFiles(filenames);
				
		UnifiedDiffCoverageFilter filter{ std::move(files), boost::none };

		filter.IsSourceFileSelected(filenames.at(0));			
		ASSERT_THROW(filter.IsSourceFileSelected(L"Test/" + filenames.at(0)), std::runtime_error);
	}
}