#include "stdafx.h"
#include "CodeCoverageRunner.hpp"

#include <boost/optional.hpp>

#include "CoverageData.hpp"
#include "Debugger.hpp"

namespace CppCoverage
{

	CoverageData CodeCoverageRunner::RunCoverage(const StartInfo& startInfo, const CoverageSettings&) const
	{
		CoverageData test;
		
		// Create processMemory object

		// Create DebugInformation filter on module outsite, use predicate for source. build coverage data inside inner loop.

		// DebugInformation use class with 2 methodes Filteer ( set module as used un Coverage data, et use reg ex) and a callback with all the data
		// Reflechir a Coverage Data before
		// CoverageData global stats
		//		Modules global stats no source selected
		//			File  stats
		// Create a class for instruction and info
		// main class
		//$$	 internal struct module, source, then line easy to build coverageData ...

		// $$ The class that manage the handle of exception must be in the ExceptionCallback class and so cannot be inside this class.
		//  Set current module and OnnewLine to fill the possible linge
		// find to set visited line
		// Func
	//	Debugger debugger;

//		debugger.Debug(startInfo, IDebugEvents&);
	//	StartInfo startInfo2(L"");

		return test;
	}
}
