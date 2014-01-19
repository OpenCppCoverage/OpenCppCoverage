#include "stdafx.h"
#include "CoverageFilter.hpp"

#include "CoverageSettings.hpp"

namespace CppCoverage
{	
	namespace
	{
		bool MatchAny(const std::wstring& str, const std::vector<boost::wregex>& regexes)
		{
			for (const auto& regEx : regexes)
			{
				if (boost::regex_match(str, regEx))
					return true;
			}

			return false;
		}
		
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
	CoverageFilter::CoverageFilter(const CoverageSettings& coverageSettings)		
	{
		auto isRegexCaseSensitiv = coverageSettings.IsRegexCaseSensitiv();

		positiveModuleRegexes_ = BuildRegexes(
			coverageSettings.GetModulesPositivePatterns(),
			isRegexCaseSensitiv);

		positiveSourceRegexes_ = BuildRegexes(
			coverageSettings.GetSourcesPositivePatterns(),
			isRegexCaseSensitiv);
	}

	//-------------------------------------------------------------------------
	bool CoverageFilter::IsModuleSelected(const std::wstring& filename) const
	{
		return MatchAny(filename, positiveModuleRegexes_);
	}

	//-------------------------------------------------------------------------
	bool CoverageFilter::IsSourceFileSelected(const std::wstring& filename) const
	{
		return MatchAny(filename, positiveSourceRegexes_);
	}

}
