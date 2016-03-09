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

#include "CppCoverage/CoverageFilterManager.hpp"
#include "CppCoverage/UnifiedDiffSettings.hpp"
#include "CppCoverage/Patterns.hpp"
#include "CppCoverage/CoverageSettings.hpp"
#include "FileFilter/UnifiedDiffCoverageFilter.hpp"
#include "FileFilter/File.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace CppCoverageTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		cov::CoverageFilterManager::UnifiedDiffCoverageFilters CreateFilter(
			const std::vector<fs::path>& paths)
		{
			cov::CoverageFilterManager::UnifiedDiffCoverageFilters filters;
			std::vector<FileFilter::File> files;

			for (const auto& path : paths)
				files.emplace_back(path);
			filters.push_back(std::make_unique<FileFilter::UnifiedDiffCoverageFilter>(std::move(files), boost::none));

			return filters;
		}

		//-------------------------------------------------------------------------
		std::unique_ptr<cov::CoverageFilterManager> CreateCoverageFilterManager(
			const std::vector<fs::path>& paths,
			const std::vector<std::wstring>& sourcePatternStrs)
		{
			cov::Patterns sourcePatterns{ false };

			for (const auto& pattern : sourcePatternStrs)
				sourcePatterns.AddSelectedPatterns(pattern);
			cov::CoverageSettings coverageSettings{ cov::Patterns{ false }, sourcePatterns };

			auto filters = CreateFilter(paths);

			return std::make_unique<cov::CoverageFilterManager>(coverageSettings, std::move(filters));
		}
	}

	//-------------------------------------------------------------------------
	TEST(CoverageFilterManagerTest, PatternsAndDiff)
	{
		const fs::path diff = L"diff";
		const std::wstring pattern = L"pattern";
		auto coverageFilterManager = CreateCoverageFilterManager({ diff }, { pattern });

		ASSERT_TRUE(coverageFilterManager->IsSourceFileSelected((pattern / diff).wstring()));
		ASSERT_FALSE(coverageFilterManager->IsSourceFileSelected(diff.wstring()));
		ASSERT_FALSE(coverageFilterManager->IsSourceFileSelected(pattern));
	}

	//-------------------------------------------------------------------------
	TEST(CoverageFilterManagerTest, TwoDiff)
	{
		const fs::path diff = L"diff";
		const fs::path diff2 = L"diff2";
		auto coverageFilterManager = CreateCoverageFilterManager({ diff, diff2 }, { L"*"});

		ASSERT_TRUE(coverageFilterManager->IsSourceFileSelected(diff.wstring()));
		ASSERT_TRUE(coverageFilterManager->IsSourceFileSelected(diff2.wstring()));
		ASSERT_FALSE(coverageFilterManager->IsSourceFileSelected(L"Unknow"));
	}
}