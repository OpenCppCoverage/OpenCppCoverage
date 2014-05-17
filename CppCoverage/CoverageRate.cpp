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