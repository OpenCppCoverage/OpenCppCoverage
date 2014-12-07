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
		void AssertLinesEquals(
			const cov::LineCoverage& line1,
			const cov::LineCoverage& line2)
		{
			ASSERT_EQ(line1.GetLineNumber(), line2.GetLineNumber());
			ASSERT_EQ(line1.HasBeenExecuted(), line2.HasBeenExecuted());
		}

		//---------------------------------------------------------------------
		void AssertFilesEquals(
			const cov::FileCoverage& file1,
			const cov::FileCoverage& file2)
		{
			ASSERT_EQ(file1.GetPath(), file2.GetPath());
			AssertContainerEqual(file1.GetLines(), file2.GetLines(), AssertLinesEquals);
		}

		//---------------------------------------------------------------------
		void AssertModulesEquals(
			const cov::ModuleCoverage& module1,
			const cov::ModuleCoverage& module2)
		{
			ASSERT_EQ(module1.GetPath(), module2.GetPath());
			AssertContainerUniquePtrEqual(module1.GetFiles(), module2.GetFiles(), AssertFilesEquals);
		}

		//---------------------------------------------------------------------
		void AssertEquals(
			const CppCoverage::CoverageData& coverageData,
			const CppCoverage::CoverageData& coverageDataRestored)
		{
			ASSERT_EQ(coverageData.GetName(), coverageDataRestored.GetName());
			ASSERT_EQ(coverageData.GetExitCode(), coverageDataRestored.GetExitCode());

			AssertContainerUniquePtrEqual(
				coverageData.GetModules(),
				coverageDataRestored.GetModules(),
				AssertModulesEquals);
		}
	}

	//-------------------------------------------------------------------------
	void CoverageDataComparer::AssertEquals(
		const CppCoverage::CoverageData&, 
		const CppCoverage::CoverageData&) const
	{

	}
}
