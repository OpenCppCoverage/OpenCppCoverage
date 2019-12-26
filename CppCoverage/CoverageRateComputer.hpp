// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "CppCoverageExport.hpp"
#include <unordered_map>
#include "CoverageRate.hpp"

namespace Plugin
{
	class ModuleCoverage;
	class FileCoverage;
	class CoverageData;
}

namespace CppCoverage
{
	class CPPCOVERAGE_DLL CoverageRateComputer
	{
	public:
		explicit CoverageRateComputer(const Plugin::CoverageData&);

		std::vector<Plugin::ModuleCoverage*> SortModulesByCoverageRate() const;
		std::vector<Plugin::FileCoverage*> SortFilesByCoverageRate(const Plugin::ModuleCoverage&) const;
		
		const CoverageRate& GetCoverageRate() const;
		const CoverageRate& GetCoverageRate(const Plugin::ModuleCoverage&) const;
		const CoverageRate& GetCoverageRate(const Plugin::FileCoverage&) const;

	private:
		CoverageRateComputer(const CoverageRateComputer&) = delete;
		CoverageRateComputer& operator=(const CoverageRateComputer&) = delete;

		void ComputeCoverageRateCache(const Plugin::CoverageData& coverageData);

		const Plugin::CoverageData& coverageData_;

		std::unordered_map<const Plugin::ModuleCoverage*, CoverageRate> moduleCoverageRate_;
		std::unordered_map<const Plugin::FileCoverage*, CoverageRate> fileCoverageRate_;
		CoverageRate coverageRate_;
	};
}