#ifndef CPPCOVERAGE_LINECOVERAGE_HEADER_GARD
#define CPPCOVERAGE_LINECOVERAGE_HEADER_GARD

#include "Export.hpp"

namespace CppCoverage
{
	class DLL LineCoverage
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

#endif
