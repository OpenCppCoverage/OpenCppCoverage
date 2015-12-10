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

namespace CppCoverage
{
	class ModuleCoverage;
	class FileCoverage;
	class CoverageData;

	class CPPCOVERAGE_DLL CoverageRateComputer
	{
	public:
		explicit CoverageRateComputer(const CoverageData&);

		std::vector<ModuleCoverage*> SortModulesByCoverageRate() const;
		std::vector<FileCoverage*> SortFilesByCoverageRate(const ModuleCoverage&) const;
		
		const CoverageRate& GetCoverageRate(const ModuleCoverage&) const;
		const CoverageRate& GetCoverageRate(const FileCoverage&) const;

	private:
		CoverageRateComputer(const CoverageRateComputer&) = delete;
		CoverageRateComputer& operator=(const CoverageRateComputer&) = delete;

		void ComputeCoverageRateCache(const CoverageData& coverageData);

		const CoverageData& coverageData_;

		std::unordered_map<const ModuleCoverage*, CoverageRate> moduleCoverageRate_;
		std::unordered_map<const FileCoverage*, CoverageRate> fileCoverageRate_;
	};
}