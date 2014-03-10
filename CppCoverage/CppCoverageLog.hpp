#pragma once

#include <iosfwd>
#include <boost/log/trivial.hpp>
#include <boost/shared_ptr.hpp>
#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	void CPPCOVERAGE_DLL SetLoggerMinSeverity(boost::log::trivial::severity_level minSeverity);
	void CPPCOVERAGE_DLL ResetLogger();
	void CPPCOVERAGE_DLL InitLoggerOstream(const boost::shared_ptr<std::ostringstream>& ostr);
}


