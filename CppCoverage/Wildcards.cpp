#include "stdafx.h"
#include "Wildcards.hpp"

#include <boost/algorithm/string.hpp>

#include "Tools/Tool.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	const std::vector<wchar_t> Wildcards::EscapedChars = 
		{ L'\\', L'^', L'$', L'.', L'+', L'(', L')', L'[', L']', L'{', L'}', L'|', L'?' };

	//-------------------------------------------------------------------------
	Wildcards::Wildcards(std::wstring str, bool isRegexCaseSensitiv)
		: originalStr_{ str }
	{
		auto flags = (isRegexCaseSensitiv) ? std::regex::basic : std::regex::icase;
		// Do not escaped '*'
		
		for (auto c : EscapedChars)
		{
			std::wstring escapedString{ c };
			boost::replace_all(str, escapedString, L"\\" + escapedString);
		}
		
		// remove useless * 
		boost::trim_if(str, [](char c){ return c == '*';});
		boost::replace_all(str, L"*", L".*");
		wildcars_.assign(str, flags);
	}

	//-------------------------------------------------------------------------
	Wildcards::Wildcards(Wildcards&& wildcards)
		: wildcars_(std::move(wildcards.wildcars_))
	{
	}

	//-------------------------------------------------------------------------
	bool Wildcards::Match(const std::wstring& str) const
	{
		return std::regex_search(str, wildcars_);
	}

	//-------------------------------------------------------------------------
	std::wostream& operator<<(std::wostream& ostr, const Wildcards& wildcards)
	{
		ostr << wildcards.originalStr_;

		return ostr;
	}
}
