#ifndef CPPCOVERAGE_CODECOVERAGERUNNER_HEADER_GARD
#define CPPCOVERAGE_CODECOVERAGERUNNER_HEADER_GARD

#include "CoverageData.hpp"

namespace CppCoverage
{
	class StartInfo;
	class CoverageSettings;

	class CodeCoverageRunner
	{
	public:
		CodeCoverageRunner() = default;

		CoverageData RunCoverage(const StartInfo&, const CoverageSettings&) const;

	private:
		CodeCoverageRunner(const CodeCoverageRunner&) = delete;
		CodeCoverageRunner& operator=(const CodeCoverageRunner&) = delete;
	};
}

#endif
