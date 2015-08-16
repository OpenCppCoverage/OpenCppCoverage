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

namespace CppCoverage
{
	class CoverageData;

	class CPPCOVERAGE_DLL CoverageDataMerger
	{
	public:
		CoverageDataMerger() = default;
		
		CoverageData Merge(const std::vector<CoverageData>&) const;
		void MergeFileCoverage(CoverageData&) const;

	private:
		CoverageDataMerger(const CoverageDataMerger&) = delete;
		CoverageDataMerger& operator=(const CoverageDataMerger&) = delete;
	};
}