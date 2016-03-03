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
#include "CoverageFilter.hpp"

#include <sstream>
#include <boost/regex.hpp>

#include "Tools/Log.hpp"

#include "CoverageSettings.hpp"
#include "Patterns.hpp"
#include "Wildcards.hpp"

namespace CppCoverage
{	
	//-------------------------------------------------------------------------
	struct WildcardCoverageFilter::Filter
	{		
		std::vector<Wildcards> selectedWildcards;
		std::vector<Wildcards> excludedWildcards;
	};

	namespace
	{
		//---------------------------------------------------------------------
		const Wildcards* MatchAny(
			const std::wstring& str, 
			const std::vector<Wildcards>& wildcardsCollection)
		{
			
			for (const auto& wildcards : wildcardsCollection)
			{				
				if (wildcards.Match(str))
					return &wildcards;
			}

			return nullptr;
		}		
				
		//---------------------------------------------------------------------
		std::vector<Wildcards> BuildWildcards(
			const std::vector<std::wstring>& regexesStr,
			bool isRegexCaseSensitiv)
		{			
			std::vector<Wildcards> wildcardsCollection;

			for (const auto& regexStr : regexesStr)
				wildcardsCollection.emplace_back(regexStr, isRegexCaseSensitiv);
						
			return wildcardsCollection;
		}	
	}

	//-------------------------------------------------------------------------
	WildcardCoverageFilter::WildcardCoverageFilter(const CoverageSettings& settings)		
	{
		moduleFilter_ = BuildFilter(settings.GetModulePatterns());
		sourceFilter_ = BuildFilter(settings.GetSourcePatterns());
	}

	//-------------------------------------------------------------------------
	WildcardCoverageFilter::~WildcardCoverageFilter()
	{
	}

	//-------------------------------------------------------------------------
	bool WildcardCoverageFilter::IsModuleSelected(const std::wstring& filename) const
	{
		std::wostringstream ostr;
		bool isSelected = Match(filename, *moduleFilter_, ostr);

		LOG_INFO << L"Module" << ostr.str();
		return isSelected;
	}

	//-------------------------------------------------------------------------
	bool WildcardCoverageFilter::IsSourceFileSelected(const std::wstring& filename) const
	{
		std::wostringstream ostr;
		bool isSelected = Match(filename, *sourceFilter_, ostr);

		LOG_DEBUG << L"Filename" << ostr.str();
		return isSelected;		
	}

	//-------------------------------------------------------------------------
	std::unique_ptr<WildcardCoverageFilter::Filter> 
		WildcardCoverageFilter::BuildFilter(const Patterns& patterns) const
	{
		std::unique_ptr<Filter> filter{ new Filter()};

		filter->selectedWildcards = BuildWildcards(
			patterns.GetSelectedPatterns(), 
			patterns.IsRegexCaseSensitiv());
		filter->excludedWildcards = BuildWildcards(
			patterns.GetExcludedPatterns(), 
			patterns.IsRegexCaseSensitiv());

		return filter;
	}

	//---------------------------------------------------------------------
	bool WildcardCoverageFilter::Match(
		const std::wstring& str,
		const Filter& filter,
		std::wostream& ostr) const
	{
		const auto* selectedRegEx = MatchAny(str, filter.selectedWildcards);

		if (!selectedRegEx)
		{
			ostr << L": " << str << L" is skipped because it matches no selected patterns";
			return false;
		}
		
		const auto* excludedRegEx = MatchAny(str, filter.excludedWildcards);

		if (excludedRegEx)
		{
			ostr << L": " << str << L" is not selected because it matches excluded pattern: " << *excludedRegEx;
			return false;
		}

		ostr << L": " << str << L" is selected because it matches selected pattern: " << *selectedRegEx;;
		return true;			
	}
}
