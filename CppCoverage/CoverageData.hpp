#ifndef CPPCOVERAGE_COVERAGEDATA_HEADER_GARD
#define CPPCOVERAGE_COVERAGEDATA_HEADER_GARD

namespace CppCoverage
{
	class CoverageData
	{
	public:
		CoverageData() = default;
		CoverageData(CoverageData&&);

	private:
		CoverageData(const CoverageData&) = delete;
		CoverageData& operator=(const CoverageData&) = delete;
	};
}

#endif
