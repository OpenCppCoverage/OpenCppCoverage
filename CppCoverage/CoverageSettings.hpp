#ifndef CPPCOVERAGE_COVERAGESETTINGS_HEADER_GARD
#define CPPCOVERAGE_COVERAGESETTINGS_HEADER_GARD

namespace CppCoverage
{
	class CoverageSettings
	{
	public:
		CoverageSettings() = default;
	private:
		CoverageSettings(const CoverageSettings&) = delete;
		CoverageSettings& operator=(const CoverageSettings&) = delete;
	};
}

#endif
