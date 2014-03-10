#include "stdafx.h"
#include "CoverageSettings.hpp"

#include "Tools/Log.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	CoverageSettings::CoverageSettings(
		const Patterns& modulePatterns,
		const Patterns& sourcePatterns)
		: modulePatterns_(modulePatterns)
		, sourcePatterns_(sourcePatterns)
	{
	}

	//-------------------------------------------------------------------------
	const Patterns& CoverageSettings::GetModulePatterns() const
	{
		return modulePatterns_;
	}
	
	//-------------------------------------------------------------------------
	const Patterns& CoverageSettings::GetSourcePatterns() const
	{
		return sourcePatterns_;
	}

}

