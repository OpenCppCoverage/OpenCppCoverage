#include "stdafx.h"

#include "CppCoverage/CoverageFilter.hpp"
#include "CppCoverage/CoverageSettings.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	TEST(CoverageFilterTest, IsModuleSelected)
	{
		cov::CoverageSettings settings;

		settings.AddModulePositivePatterns(L"123.*");
		settings.AddModulePositivePatterns(L".*456.*");

		cov::CoverageFilter filter{ settings };

		ASSERT_TRUE(filter.IsModuleSelected(L"1234"));
		ASSERT_TRUE(filter.IsModuleSelected(L"3456"));
		ASSERT_FALSE(filter.IsModuleSelected(L"34756"));
	}

	TEST(CoverageFilterTest, IsSourceSelected)
	{
		cov::CoverageSettings settings;

		settings.AddSourcePositivePatterns(L"123.*");

		cov::CoverageFilter filter{ settings };

		ASSERT_TRUE(filter.IsSourceFileSelected(L"1234"));
		ASSERT_FALSE(filter.IsSourceFileSelected(L"34756"));
	}
}