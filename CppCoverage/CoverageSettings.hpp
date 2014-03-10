#pragma once

#include <string>
#include <vector>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class Patterns;

	class CPPCOVERAGE_DLL CoverageSettings
	{	
	public:
		CoverageSettings(const Patterns& modulePatterns, const Patterns& sourcePatterns);

		const Patterns& GetModulePatterns() const;
		const Patterns& GetSourcePatterns() const;
		
	private:
		CoverageSettings(const CoverageSettings&) = delete;
		CoverageSettings& operator=(const CoverageSettings&) = delete;

	private:
		const Patterns& modulePatterns_;
		const Patterns& sourcePatterns_;
	};
}


