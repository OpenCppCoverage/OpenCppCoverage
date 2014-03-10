#include "stdafx.h"
#include "Patterns.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	Patterns::Patterns(bool isRegexCaseSensitiv)
		: isRegexCaseSensitiv_(isRegexCaseSensitiv)
	{
	}
	
	//-------------------------------------------------------------------------
	Patterns::Patterns(Patterns&& pattern)
		: isRegexCaseSensitiv_(pattern.isRegexCaseSensitiv_)
	{
		std::swap(positivePatterns_, pattern.positivePatterns_);
		std::swap(negativePatterns_, pattern.negativePatterns_);
	}

	//-------------------------------------------------------------------------
	void Patterns::AddPositivePatterns(const std::wstring& pattern)
	{
		positivePatterns_.push_back(pattern);
	}

	//-------------------------------------------------------------------------
	const Patterns::T_Patterns& Patterns::GetPositivePatterns() const
	{
		return positivePatterns_;
	}

	//-------------------------------------------------------------------------
	void Patterns::AddNegativePatterns(const std::wstring& pattern)
	{
		negativePatterns_.push_back(pattern);
	}
	
	//-------------------------------------------------------------------------
	const Patterns::T_Patterns& Patterns::GetNegativePatterns() const
	{
		return negativePatterns_;
	}

	//-------------------------------------------------------------------------
	bool Patterns::IsRegexCaseSensitiv() const
	{
		return isRegexCaseSensitiv_;
	}
}
