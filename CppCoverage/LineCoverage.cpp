#include "stdafx.h"
#include "LineCoverage.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	LineCoverage::LineCoverage(unsigned int lineNumber, bool hasBeenExecuted)
		: lineNumber_(lineNumber)
		, hasBeenExecuted_(hasBeenExecuted)
	{
	}

	//-------------------------------------------------------------------------
	unsigned int LineCoverage::GetLineNumber() const
	{
		return lineNumber_;
	}

	//-------------------------------------------------------------------------
	bool LineCoverage::HasBeenExecuted() const
	{
		return hasBeenExecuted_;
	}
}
