#ifndef CPPCOVERAGE_COVERAGEFILTER_HEADER_GARD
#define CPPCOVERAGE_COVERAGEFILTER_HEADER_GARD

#include <string>

namespace CppCoverage
{
	class CoverageFilter
	{
	public:
		CoverageFilter() = default;

		bool IsModuleSelected(const std::wstring& filename) const;
		bool IsSourceFileSelected(const std::wstring& filename) const;

	private:
		CoverageFilter(const CoverageFilter&) = delete;
		CoverageFilter& operator=(const CoverageFilter&) = delete;
	};
}

#endif
