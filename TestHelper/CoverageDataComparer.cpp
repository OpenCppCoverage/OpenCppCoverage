// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"

#include "CoverageDataComparer.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"

#include "Container.hpp"

namespace cov = CppCoverage;

namespace TestHelper
{
	namespace
	{
		//---------------------------------------------------------------------
		template <typename T>
		void AssertEqual(T&& v1, T&& v2)
		{
			ASSERT_EQ(v1, v2);
			if (v1 != v2)
				throw std::runtime_error("Values are not equals.");
		}

		//---------------------------------------------------------------------
		void AssertLinesEquals(
			const cov::LineCoverage& line1,
			const cov::LineCoverage& line2)
		{
			AssertEqual(line1.GetLineNumber(), line2.GetLineNumber());
			AssertEqual(line1.HasBeenExecuted(), line2.HasBeenExecuted());
		}

		//---------------------------------------------------------------------
		void AssertFilesEquals(
			const cov::FileCoverage& file1,
			const cov::FileCoverage& file2)
		{
			AssertEqual(file1.GetPath(), file2.GetPath());
			AssertContainerEqual(file1.GetLines(), file2.GetLines(), AssertLinesEquals);
		}

		//---------------------------------------------------------------------
		void AssertModulesEquals(
			const cov::ModuleCoverage& module1,
			const cov::ModuleCoverage& module2)
		{
			AssertEqual(module1.GetPath(), module2.GetPath());
			AssertContainerUniquePtrEqual(module1.GetFiles(), module2.GetFiles(), AssertFilesEquals);
		}
	}

	//-------------------------------------------------------------------------
	void CoverageDataComparer::AssertEquals(
		const CppCoverage::CoverageData& coverageData,
		const CppCoverage::CoverageData& coverageDataRestored) const
	{
		AssertEqual(coverageData.GetName(), coverageDataRestored.GetName());
		AssertEqual(coverageData.GetExitCode(), coverageDataRestored.GetExitCode());

		AssertContainerUniquePtrEqual(
			coverageData.GetModules(),
			coverageDataRestored.GetModules(),
			AssertModulesEquals);
	}

	//---------------------------------------------------------------------
	void CoverageDataComparer::AssertEquals(
		const cov::ModuleCoverage* module1,
		const cov::ModuleCoverage* module2) const
	{
		if (!module1 || !module2)
			throw std::runtime_error("Module is null.");
		AssertModulesEquals(*module1, *module2);
	}
}
