#pragma once

#include <string>
#include <vector>
#include <memory>

#include "CppCoverageExport.hpp"
#include "CoverageRate.hpp"

namespace CppCoverage
{
	class ModuleCoverage;

	class CPPCOVERAGE_DLL CoverageData
	{
	public:
		typedef std::vector<std::unique_ptr<ModuleCoverage>> T_ModuleCoverageCollection;

	public:
		explicit CoverageData(const std::wstring& name, int exitCode);
		~CoverageData();

		CoverageData(CoverageData&&);

		ModuleCoverage& AddModule(const std::wstring& name);
		void ComputeCoverageRate();
		const CoverageRate& GetCoverageRate() const;

		const T_ModuleCoverageCollection& GetModules() const;
		const std::wstring& GetName() const;

		int GetExitCode() const;

	private:
		CoverageData(const CoverageData&) = delete;
		CoverageData& operator=(const CoverageData&) = delete;

	private:
		T_ModuleCoverageCollection modules_;
		std::wstring name_;
		int exitCode_;
		CoverageRate coverageRate_;
	};
}


