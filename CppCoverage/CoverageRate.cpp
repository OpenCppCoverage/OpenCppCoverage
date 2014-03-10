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
	int CoverageRate::GetUnexecutedLinesCount() const
	{
		return unexecutedLinesCount_;
	}
}