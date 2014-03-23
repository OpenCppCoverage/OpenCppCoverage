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
		explicit Patterns(bool isRegexCaseSensitiv);
		Patterns(const Patterns&) = default;
		Patterns(Patterns&&);

		void AddPositivePatterns(const std::wstring&);
		const T_Patterns& GetPositivePatterns() const;

		void AddNegativePatterns(const std::wstring&);
		const T_Patterns& GetNegativePatterns() const;

		bool IsRegexCaseSensitiv() const;

		Patterns& operator=(const Patterns&) = default;
		
	private:
		T_Patterns positivePatterns_;
		T_Patterns negativePatterns_;
		bool isRegexCaseSensitiv_;
	};
}

