#include "stdafx.h"
#include "CoverageFilter.hpp"

#include <sstream>
#include <boost/regex.hpp>

#include "Tools/Log.hpp"

#include "CoverageSettings.hpp"
#include "Patterns.hpp"

namespace CppCoverage
{	
	//-------------------------------------------------------------------------
	struct CoverageFilter::Filter
	{		
		std::vector<boost::wregex> selectedRegexes;
		std::vector<boost::wregex> excludedRegexes;
	};

	namespace
	{
		//---------------------------------------------------------------------
		const boost::wregex* MatchAny(
			const std::wstring& str, 
			const std::vector<boost::wregex>& regexes)
		{
			for (const auto& regEx : regexes)
			{
				if (boost::regex_match(str, regEx))
					return &regEx;				
			}

			return nullptr;
		}		
		
		//---------------------------------------------------------------------
		std::vector<boost::wregex> BuildRegexes(
			const std::vector<std::wstring>& regexesStr,
			bool isRegexCaseSensitiv)
		{
			auto flags = (isRegexCaseSensitiv) ? boost::regex::basic : boost::regex::icase;
			std::vector<boost::wregex> regexes;

			for (const auto& regexStr : regexesStr)
				regexes.emplace_back(regexStr, flags);

			return regexes;
		}	
	}

	//-------------------------------------------------------------------------
	CoverageFilter::CoverageFilter(const CoverageSettings& settings)		
	{
		moduleFilter_ = BuildFilter(settings.GetModulePatterns());
		sourceFilter_ = BuildFilter(settings.GetSourcePatterns());
	}

	//-------------------------------------------------------------------------
	CoverageFilter::~CoverageFilter()
	{
	}

	//-------------------------------------------------------------------------
	bool CoverageFilter::IsModuleSelected(const std::wstring& filename) const
	{
		std::wostringstream ostr;
		bool isSelected = Match(filename, *moduleFilter_, ostr);

		LOG_INFO << L"Module" << ostr.str();
		return isSelected;
	}

	//-------------------------------------------------------------------------
	bool CoverageFilter::IsSourceFileSelected(const std::wstring& filename) const
	{
		std::wostringstream ostr;
		bool isSelected = Match(filename, *sourceFilter_, ostr);

		LOG_DEBUG << L"Filename" << ostr.str();
		return isSelected;		
	}

	//-------------------------------------------------------------------------
	std::unique_ptr<CoverageFilter::Filter> 
		CoverageFilter::BuildFilter(const Patterns& patterns) const
	{
		std::unique_ptr<Filter> filter{ new Filter()};

		filter->selectedRegexes = BuildRegexes(
			patterns.GetSelectedPatterns(), 
			patterns.IsRegexCaseSensitiv());
		filter->excludedRegexes = BuildRegexes(
			patterns.GetExcludedPatterns(), 
			patterns.IsRegexCaseSensitiv());

		return filter;
	}

	//---------------------------------------------------------------------
	bool CoverageFilter::Match(
		const std::wstring& str,
		const Filter& filter,
		std::wostream& ostr) const
	{
		const auto* selectedRegEx = MatchAny(str, filter.selectedRegexes);

		if (!selectedRegEx)
		{
			ostr << L": " << str << L" is skipped. Match no selected patterns";
			return false;
		}
		// $$ no wargningfor normal message (same line for same file) for example
		const auto* excludedRegEx = MatchAny(str, filter.excludedRegexes);

		if (excludedRegEx)
		{
			ostr << L": " << str << L" is not selected because it matchs excluded pattern: " << *excludedRegEx;
			return false;
		}

		ostr << L": " << str << L" is selected because it matchs selected pattern: " << *selectedRegEx;;
		return true;			
	}
}
