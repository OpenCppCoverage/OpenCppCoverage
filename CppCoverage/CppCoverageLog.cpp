#include "stdafx.h"
#include "CppCoverageLog.hpp"

#include <boost/log/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/expressions.hpp>

#include <boost/shared_array.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	void SetLoggerMinSeverity(const boost::log::trivial::severity_level minSeverity)
	{
		auto filter = logging::trivial::severity >= minSeverity;
		logging::core::get()->set_filter(filter);
	}

	//-------------------------------------------------------------------------
	void InitLoggerOstream(const boost::shared_ptr<std::ostringstream>& ostr)
	{		
		typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
		auto sink = boost::make_shared<text_sink>();
		auto& backend = sink->locked_backend();

		backend->add_stream(ostr);
		backend->auto_flush(true);
		logging::core::get()->add_sink(sink);
	}

	//-------------------------------------------------------------------------
	void ResetLogger()
	{
		logging::core::get()->remove_all_sinks();		
	}
}