#pragma once

#include <vector>
#include <string>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL Patterns
	{
	public:
		typedef std::vector<std::wstring> T_Patterns;

	public:
		explicit Patterns(bool isRegexCaseSensitiv = false);
		Patterns(const Patterns&) = default;
		Patterns(Patterns&&);

		void AddSelectedPatterns(const std::wstring&);
		const T_Patterns& GetSelectedPatterns() const;

		void AddExcludedPatterns(const std::wstring&);
		const T_Patterns& GetExcludedPatterns() const;

		bool IsRegexCaseSensitiv() const;

		Patterns& operator=(const Patterns&) = default;
		
		CPPCOVERAGE_DLL friend std::wostream& operator<<(std::wostream&, const Patterns&);
	
	private:
		T_Patterns selectedPatterns_;
		T_Patterns excludedPatterns_;
		bool isRegexCaseSensitiv_;
	};
}

