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

#include "TestHelperExport.hpp"
#include <memory>

namespace CppCoverage
{
	class CoverageData;
	class ModuleCoverage;
}

namespace TestHelper
{
	class TEST_HELPER_DLL CoverageDataComparer
	{
	public:
		CoverageDataComparer() = default;

		void AssertEquals(const CppCoverage::CoverageData&, const CppCoverage::CoverageData&) const;
		void AssertEquals(const CppCoverage::ModuleCoverage*, const CppCoverage::ModuleCoverage*) const;

		using ModuleCoveragePtr = std::unique_ptr<CppCoverage::ModuleCoverage>;
		using ModuleCoverageCollection = std::vector<ModuleCoveragePtr>;

		bool IsFirstModuleContainsSecond(
			const ModuleCoveragePtr& module1,
			const ModuleCoveragePtr& module2) const;

		bool IsFirstCollectionContainsSecond(
			const ModuleCoverageCollection& container1,
			const ModuleCoverageCollection& container2) const;
	private:
		CoverageDataComparer(const CoverageDataComparer&) = delete;
		CoverageDataComparer& operator=(const CoverageDataComparer&) = delete;
	};
}