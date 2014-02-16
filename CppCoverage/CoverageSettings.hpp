#ifndef CPPCOVERAGE_COVERAGESETTINGS_HEADER_GARD
#define CPPCOVERAGE_COVERAGESETTINGS_HEADER_GARD

#include <string>
#include <vector>

#include "Export.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL CoverageSettings
	{
	public:
		typedef std::vector<std::wstring> T_Patterns;

	public:
		CoverageSettings();

		void AddModulePositivePatterns(const std::wstring&);
		const T_Patterns& GetModulesPositivePatterns() const;

		void AddSourcePositivePatterns(const std::wstring&);
		const T_Patterns& GetSourcesPositivePatterns() const;

		void SetIsRegexCaseSensitiv(bool);
		bool IsRegexCaseSensitiv() const;

	private:
		CoverageSettings(const CoverageSettings&) = delete;
		CoverageSettings& operator=(const CoverageSettings&) = delete;

	private:
		T_Patterns modulesPositivePatterns_;
		T_Patterns sourcesPositivePatterns_;
		bool isRegexCaseSensitiv_;

	};
}

#endif
