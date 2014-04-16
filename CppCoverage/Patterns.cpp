#include "stdafx.h"
#include "Patterns.hpp"

namespace CppCoverage
{
	namespace
	{
		//-------------------------------------------------------------------------
		std::wostream& operator<<(std::wostream& ostr, const Patterns::T_Patterns& patterns)
		{
			for (const auto& pattern : patterns)
				ostr << pattern << ' ';
			return ostr;
		}
	}

	//-------------------------------------------------------------------------
	Patterns::Patterns(bool isRegexCaseSensitiv)
		: isRegexCaseSensitiv_(isRegexCaseSensitiv)
	{
	}
	
	//-------------------------------------------------------------------------
	Patterns::Patterns(Patterns&& pattern)
		: isRegexCaseSensitiv_(pattern.isRegexCaseSensitiv_)
	{
		std::swap(selectedPatterns_, pattern.selectedPatterns_);
		std::swap(excludedPatterns_, pattern.excludedPatterns_);
	}

	//-------------------------------------------------------------------------
	void Patterns::AddSelectedPatterns(const std::wstring& pattern)
	{
		selectedPatterns_.push_back(pattern);
	}

	//-------------------------------------------------------------------------
	const Patterns::T_Patterns& Patterns::GetSelectedPatterns() const
	{
		return selectedPatterns_;
	}

	//-------------------------------------------------------------------------
	void Patterns::AddExcludedPatterns(const std::wstring& pattern)
	{
		excludedPatterns_.push_back(pattern);
	}
	
	//-------------------------------------------------------------------------
	const Patterns::T_Patterns& Patterns::GetExcludedPatterns() const
	{
		return excludedPatterns_;
	}

	//-------------------------------------------------------------------------
	bool Patterns::IsRegexCaseSensitiv() const
	{
		return isRegexCaseSensitiv_;
	}
	
	//-------------------------------------------------------------------------
	std::wostream& operator<<(std::wostream& ostr, const Patterns& patterns)
	{
		ostr << "Selected: " << patterns.selectedPatterns_;
		ostr << "Excluded: " << patterns.excludedPatterns_;

		return ostr;
	}
}
