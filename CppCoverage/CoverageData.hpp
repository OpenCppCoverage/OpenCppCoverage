#ifndef CPPCOVERAGE_COVERAGEDATA_HEADER_GARD
#define CPPCOVERAGE_COVERAGEDATA_HEADER_GARD

#include <string>
#include <vector>
#include <memory>

#include "Export.hpp"

namespace CppCoverage
{
	class ModuleCoverage;

	class CPPCOVERAGE_DLL CoverageData
	{
	public:
		typedef std::vector<std::unique_ptr<ModuleCoverage>> T_ModuleCoverageCollection;

	public:
		explicit CoverageData(const std::wstring& name);
		~CoverageData();

		CoverageData(CoverageData&&);

		ModuleCoverage& AddModule(const std::wstring& name);

		const T_ModuleCoverageCollection& GetModules() const;
		const std::wstring& GetName() const;

	private:
		CoverageData(const CoverageData&) = delete;
		CoverageData& operator=(const CoverageData&) = delete;

	private:
		T_ModuleCoverageCollection modules_;
		std::wstring name_;
	};
}

#endif
