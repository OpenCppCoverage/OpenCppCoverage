#include "stdafx.h"
#include "CoverageSettings.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	CoverageSettings::CoverageSettings()
		: isRegexCaseSensitiv_(false)
	{
	}	

	//-------------------------------------------------------------------------
	void CoverageSettings::AddModulePositivePatterns(const std::wstring& pattern)
	{
		modulesPositivePatterns_.push_back(pattern);
	}

	//-------------------------------------------------------------------------
	const CoverageSettings::T_Patterns& CoverageSettings::GetModulesPositivePatterns() const
	{
		return modulesPositivePatterns_;
	}

	//-------------------------------------------------------------------------
	void CoverageSettings::AddSourcePositivePatterns(const std::wstring& pattern)
	{
		sourcesPositivePatterns_.push_back(pattern);
	}

	//-------------------------------------------------------------------------
	const CoverageSettings::T_Patterns& CoverageSettings::GetSourcesPositivePatterns() const
	{
		return sourcesPositivePatterns_;
	}

	//-------------------------------------------------------------------------
	void CoverageSettings::SetIsRegexCaseSensitiv(bool isRegexCaseSensitiv)
	{
		isRegexCaseSensitiv_ = isRegexCaseSensitiv;
	}

	//-------------------------------------------------------------------------
	bool CoverageSettings::IsRegexCaseSensitiv() const
	{
		return isRegexCaseSensitiv_;
	}
}

