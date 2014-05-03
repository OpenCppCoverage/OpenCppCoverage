#include "stdafx.h"

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
				std::function<bool(const cov::CoverageFilter&, const std::wstring&)> isSelected)
			{
				cov::CoverageFilter filter{settings};

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
		CheckSelection(settings, [](const cov::CoverageFilter& filter, const std::wstring& str)
		{
			return filter.IsModuleSelected(str);
		});
	}

	//-------------------------------------------------------------------------
	TEST_F(CoverageFilterTest, IsSourceSelected)
	{
		cov::CoverageSettings settings{ emptyPatterns_, defaultPatterns_ };
		CheckSelection(settings, [](const cov::CoverageFilter& filter, const std::wstring& str)
		{
			return filter.IsSourceFileSelected(str);
		});		
	}
}