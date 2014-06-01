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
#include "CoverageRate.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	CoverageRate::CoverageRate()
		: executedLinesCount_{ 0 }
	, unexecutedLinesCount_{ 0 }
	{
	}

	//-------------------------------------------------------------------------
	void CoverageRate::SetExecutedLinesCount(int executedLinesCount)
	{
		executedLinesCount_ = executedLinesCount;
	}

	//-------------------------------------------------------------------------
	void CoverageRate::SetUnexecutedLinesCount(int unexecutedLinesCount)
	{
		unexecutedLinesCount_ = unexecutedLinesCount;
	}

	//-------------------------------------------------------------------------
	int CoverageRate::GetExecutedLinesCount() const
	{
		return executedLinesCount_;
	}

	//-------------------------------------------------------------------------
	int CoverageRate::GetUnExecutedLinesCount() const
	{
		return unexecutedLinesCount_;
	}

	//-------------------------------------------------------------------------
	int CoverageRate::GetTotalLinesCount() const
	{
		return executedLinesCount_ + unexecutedLinesCount_;
	}

	//-------------------------------------------------------------------------
	int CoverageRate::GetPercentRate() const
	{
		auto totalLines = executedLinesCount_ + unexecutedLinesCount_;

		if (totalLines == 0)
			return 100;
		return (executedLinesCount_ * 100) / (executedLinesCount_ + unexecutedLinesCount_);
	}
}