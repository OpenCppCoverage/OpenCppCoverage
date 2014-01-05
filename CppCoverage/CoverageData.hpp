#ifndef CPPCOVERAGE_COVERAGEDATA_HEADER_GARD
#define CPPCOVERAGE_COVERAGEDATA_HEADER_GARD

#include <string>
#include <vector>
#include <memory>

#include "Export.hpp"

namespace CppCoverage
{
	class ModuleCoverage;

	class DLL CoverageData
	{
	public:
		typedef std::vector<std::unique_ptr<ModuleCoverage>> T_ModuleCoverageCollection;

	public:
		CoverageData();
		~CoverageData();

		CoverageData(CoverageData&&);

		ModuleCoverage& AddModule(const std::wstring& name);

		const T_ModuleCoverageCollection& GetModules() const;

	private:
		CoverageData(const CoverageData&) = delete;
		CoverageData& operator=(const CoverageData&) = delete;

	private:
		T_ModuleCoverageCollection modules_;
	};
}

#endif
