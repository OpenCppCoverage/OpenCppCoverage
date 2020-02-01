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
#include "Patterns.hpp"

#include <iostream>

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
