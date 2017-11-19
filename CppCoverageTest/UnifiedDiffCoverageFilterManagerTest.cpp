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

#include "CppCoverage/UnifiedDiffCoverageFilterManager.hpp"
#include "CppCoverage/UnifiedDiffSettings.hpp"
#include "FileFilter/UnifiedDiffCoverageFilter.hpp"
#include "FileFilter/FileInfo.hpp"
#include "FileFilter/LineInfo.hpp"
#include "FileFilter/File.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace CppCoverageTest
{
	namespace
	{
		using UnifiedDiffCoverageFilters = cov::UnifiedDiffCoverageFilterManager::UnifiedDiffCoverageFilters;

		//-------------------------------------------------------------------------
		UnifiedDiffCoverageFilters CreateFilter(
			const std::vector<fs::path>& paths,
			const std::vector<int>& selectedLines = {})
		{
			UnifiedDiffCoverageFilters filters;
			std::vector<FileFilter::File> files;

			for (const auto& path : paths)
			{
				FileFilter::File file{ path };
				file.AddSelectedLines(selectedLines);
				files.emplace_back(std::move(file));
			}
			filters.push_back(std::make_unique<FileFilter::UnifiedDiffCoverageFilter>(
				std::move(files), boost::none));

			return filters;
		}

		//-------------------------------------------------------------------------
		std::unique_ptr<cov::UnifiedDiffCoverageFilterManager> CreateFilterManager(
			UnifiedDiffCoverageFilters&& filters)
		{
			return std::make_unique<cov::UnifiedDiffCoverageFilterManager>(std::move(filters));
		}

		//-------------------------------------------------------------------------
		std::unique_ptr<cov::UnifiedDiffCoverageFilterManager> CreateFilterManager(
			const std::vector<fs::path>& paths)
		{
			auto filters = CreateFilter(paths);
			return CreateFilterManager(std::move(filters));
		}

		//-------------------------------------------------------------------------
		size_t GetUnmatchPathsCount(size_t pathCount, size_t maxUnmatchPaths)
		{
			std::vector<fs::path> paths;

			for (size_t i = 0; i < pathCount; ++i)
				paths.push_back("path" + std::to_string(i));
			auto filterManager = CreateFilterManager(paths);
			
			auto messageLineCount = filterManager->ComputeWarningMessageLines(0).size();
			return filterManager->ComputeWarningMessageLines(maxUnmatchPaths).size() - messageLineCount;
		}
	}

	//-------------------------------------------------------------------------
	TEST(UnifiedDiffCoverageFilterManagerTest, TwoDiff)
	{
		const fs::path diff = L"diff";
		const fs::path diff2 = L"diff2";
		auto filterManager = CreateFilterManager({ diff, diff2 });

		ASSERT_TRUE(filterManager->IsSourceFileSelected(diff.wstring()));
		ASSERT_TRUE(filterManager->IsSourceFileSelected(diff2.wstring()));
		ASSERT_FALSE(filterManager->IsSourceFileSelected(L"Unknow"));
	}

	//-------------------------------------------------------------------------
	TEST(UnifiedDiffCoverageFilterManagerTest, GetWarningMessageLine)
	{
		size_t pathCount = 10;
		size_t maxUnmatchPaths = 20;

		ASSERT_EQ(pathCount - 1, GetUnmatchPathsCount(pathCount, maxUnmatchPaths));
	}

	//-------------------------------------------------------------------------
	TEST(UnifiedDiffCoverageFilterManagerTest, GetWarningMessageLineTrunc)
	{
		size_t pathCount = 10;
		size_t maxUnmatchPaths = 5;

		ASSERT_EQ(maxUnmatchPaths, GetUnmatchPathsCount(pathCount, maxUnmatchPaths));
	}

	//-------------------------------------------------------------------------
	bool IsLineSelected(
		int lineNumber,
		const std::vector<int>& selectedLines)
	{
		const fs::path filename = L"diff";
		auto filters = CreateFilter({ filename }, selectedLines);
		auto filterManager = CreateFilterManager(std::move(filters));

		std::vector<FileFilter::LineInfo> lineInfoColllection;

		for (auto line : selectedLines)
			lineInfoColllection.emplace_back(line, 0, 0);
		FileFilter::FileInfo fileInfo{ filename, std::move(lineInfoColllection) };
		FileFilter::LineInfo lineInfo{ lineNumber, 0, 0 };

		return filterManager->IsLineSelected(fileInfo, lineInfo);
	}

	//-------------------------------------------------------------------------
	TEST(UnifiedDiffCoverageFilterManagerTest, IsLineSelected)
	{
		const std::vector<int> selectedLines = { 3, 5, 10 };

		ASSERT_TRUE(IsLineSelected(3, selectedLines));
		ASSERT_TRUE(IsLineSelected(5, selectedLines));
		ASSERT_TRUE(IsLineSelected(10, selectedLines));
		ASSERT_FALSE(IsLineSelected(6, { 7 }));
		ASSERT_FALSE(IsLineSelected(4, {}));
		ASSERT_TRUE(IsLineSelected(4, { 3 }));
	}
}