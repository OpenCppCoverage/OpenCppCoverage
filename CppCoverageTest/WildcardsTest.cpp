#include "stdafx.h"

#include "CppCoverage/Wildcards.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	//-------------------------------------------------------------------------
	TEST(WildcardsTest, BasicMatch) 
	{		
		ASSERT_TRUE(cov::Wildcards(L"abc").Match(L"abc"));
		ASSERT_TRUE(cov::Wildcards(L"a").Match(L"abc"));
		ASSERT_TRUE(cov::Wildcards(L"ac").Match(L"bac"));				
	}

	//-------------------------------------------------------------------------
	TEST(WildcardsTest, BasicNoMatch)
	{
		ASSERT_FALSE(cov::Wildcards(L"ab").Match(L"aa"));
		ASSERT_FALSE(cov::Wildcards(L"b").Match(L"a"));
	}

	//-------------------------------------------------------------------------
	TEST(WildcardsTest, StarsMatch)
	{
		ASSERT_TRUE(cov::Wildcards(L"*").Match(L"aa"));
		ASSERT_TRUE(cov::Wildcards(L"b*").Match(L"ba"));
		ASSERT_TRUE(cov::Wildcards(L"*b").Match(L"ab"));
	}

	//-------------------------------------------------------------------------
	TEST(WildcardsTest, StarsNoMatch)
	{
		ASSERT_TRUE(cov::Wildcards(L"*").Match(L"aa"));
		ASSERT_TRUE(cov::Wildcards(L"b*").Match(L"b"));
		ASSERT_TRUE(cov::Wildcards(L"*b*").Match(L"bbb"));
	}
	
	//-------------------------------------------------------------------------
	TEST(WildcardsTest, SpecialCharMatch)
	{		
		std::wstring specialChars{ cov::Wildcards::EscapedChars.begin(), cov::Wildcards::EscapedChars.end() };
		ASSERT_TRUE(cov::Wildcards(specialChars).Match(specialChars));
	}

	//-------------------------------------------------------------------------
	TEST(WildcardsTest, Ostream)
	{
		std::wstring specialChars{ cov::Wildcards::EscapedChars.begin(), cov::Wildcards::EscapedChars.end() };
		std::wostringstream ostr;

		ostr << cov::Wildcards{specialChars};
		ASSERT_EQ(specialChars, ostr.str());
	}
}
