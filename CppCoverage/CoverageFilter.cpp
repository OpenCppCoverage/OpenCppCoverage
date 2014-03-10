#include "stdafx.h"
#include "CoverageFilter.hpp"

#include <sstream>

#include "Tools/Log.hpp"

#include "CoverageSettings.hpp"
#include "Patterns.hpp"

namespace CppCoverage
{	
	//-------------------------------------------------------------------------
	struct CoverageFilter::Filter
	{
		std::vector<boost::wregex> positiveRegexes;
		std::vector<boost::wregex> negativeRegexes;
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

		filter->positiveRegexes = BuildRegexes(
			patterns.GetPositivePatterns(), 
			patterns.IsRegexCaseSensitiv());
		filter->negativeRegexes = BuildRegexes(
			patterns.GetNegativePatterns(), 
			patterns.IsRegexCaseSensitiv());

		return filter;
	}

	//---------------------------------------------------------------------
	bool CoverageFilter::Match(
		const std::wstring& str,
		const Filter& filter,
		std::wostream& ostr) const
	{
		const auto* positivRegEx = MatchAny(str, filter.positiveRegexes);

		if (!positivRegEx)
		{
			ostr << L": " << str << L" is skipped. Match no positiv patterns";
			return false;
		}

		const auto* negativeRegEx = MatchAny(str, filter.negativeRegexes);

		if (negativeRegEx)
		{
			ostr << L": " << str << L" is not selected because it matchs negativ pattern: " 
				<< negativeRegEx->str();
			return false;
		}

		ostr << L": " << str << L" is selected because it matchs positiv pattern: " 
			<< positivRegEx->str();
		return true;			
	}
}
