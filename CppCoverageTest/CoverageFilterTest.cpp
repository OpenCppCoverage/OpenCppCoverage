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

#include <functional>

#include "CppCoverage/CoverageFilter.hpp"
#include "CppCoverage/CoverageSettings.hpp"
#include "CppCoverage/Patterns.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		struct CoverageFilterTest : public ::testing::Test
		{
			//---------------------------------------------------------------------
			CoverageFilterTest()
			: emptyPatterns_{}
			, defaultPatterns_{BuildDefaultPatterns()}
			{

			}

			//---------------------------------------------------------------------
			cov::Patterns BuildDefaultPatterns()
			{
				cov::Patterns patterns{false};

				patterns.AddSelectedPatterns(L"a*b");
				patterns.AddExcludedPatterns(L"3");

				return patterns;
			}

			//---------------------------------------------------------------------
			void CheckSelection(
				cov::CoverageSettings& settings, 
				std::function<bool(const cov::WildcardCoverageFilter&, const std::wstring&)> isSelected)
			{
				cov::WildcardCoverageFilter filter{settings};

				ASSERT_FALSE(isSelected(filter, L"aa"));
				ASSERT_FALSE(isSelected(filter, L"a3b"));
				ASSERT_TRUE(isSelected(filter, L"ab"));			
			}	

			cov::Patterns emptyPatterns_;
			cov::Patterns defaultPatterns_;
		};
	}

	//-------------------------------------------------------------------------
	TEST_F(CoverageFilterTest, IsModuleSelected)
	{
		cov::CoverageSettings settings{ defaultPatterns_, emptyPatterns_ };
		CheckSelection(settings, [](const cov::WildcardCoverageFilter& filter, const std::wstring& str)
		{
			return filter.IsModuleSelected(str);
		});
	}

	//-------------------------------------------------------------------------
	TEST_F(CoverageFilterTest, IsSourceSelected)
	{
		cov::CoverageSettings settings{ emptyPatterns_, defaultPatterns_ };
		CheckSelection(settings, [](const cov::WildcardCoverageFilter& filter, const std::wstring& str)
		{
			return filter.IsSourceFileSelected(str);
		});		
	}
}