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
