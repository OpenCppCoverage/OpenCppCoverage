#ifndef CPPCOVERAGE_COVERAGEFILTER_HEADER_GARD
#define CPPCOVERAGE_COVERAGEFILTER_HEADER_GARD

#include <string>
#include <vector>
#include <boost/regex.hpp>

#include "Export.hpp"

namespace CppCoverage
{
	class CoverageSettings;

	class DLL CoverageFilter
	{
	public:
		explicit CoverageFilter(const CoverageSettings&);

		bool IsModuleSelected(const std::wstring& filename) const;
		bool IsSourceFileSelected(const std::wstring& filename) const;

	private:
		CoverageFilter(const CoverageFilter&) = delete;
		CoverageFilter& operator=(const CoverageFilter&) = delete;

	private:
		std::vector<boost::wregex> positiveModuleRegexes_;
		std::vector<boost::wregex> positiveSourceRegexes_;
	};
}

#endif
