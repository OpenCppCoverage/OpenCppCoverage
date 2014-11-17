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

#include "CppCoverage/CoverageRate.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	//-------------------------------------------------------------------------
	TEST(CoverageRateTest, BasicAccessors)
	{
		const int executedLinesCount = 42;
		const int unexecutedLinesCount = 10;

		cov::CoverageRate rate{ executedLinesCount, unexecutedLinesCount };

		ASSERT_EQ(executedLinesCount + unexecutedLinesCount, rate.GetTotalLinesCount());
		ASSERT_EQ((100 * executedLinesCount) / (executedLinesCount + unexecutedLinesCount), rate.GetPercentRate());
	}

	//-------------------------------------------------------------------------
	TEST(CoverageRateTest, Add)
	{		
		cov::CoverageRate rate1{ 42, 10 };
		cov::CoverageRate rate2{ 12, 53 };

		cov::CoverageRate sum{ rate1 };

		sum += rate2;

		ASSERT_EQ(rate1.GetExecutedLinesCount() + rate2.GetExecutedLinesCount(), sum.GetExecutedLinesCount());
		ASSERT_EQ(rate1.GetUnExecutedLinesCount() + rate2.GetUnExecutedLinesCount(), sum.GetUnExecutedLinesCount());
	}
}