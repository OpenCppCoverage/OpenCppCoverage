#pragma once

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL LineCoverage
	{
	public:
		LineCoverage(unsigned int lineNumber, bool hasBeenExecuted);
		LineCoverage(const LineCoverage&) = default;
		LineCoverage& operator=(const LineCoverage&) = default;

		unsigned int GetLineNumber() const;
		bool HasBeenExecuted() const;
		
	private:
		unsigned int lineNumber_;
		bool hasBeenExecuted_;
	};
}


