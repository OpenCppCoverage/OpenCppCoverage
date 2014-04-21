#include "stdafx.h"
#include "Log.hpp"

#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#include <boost/date_time.hpp>
#include <boost/shared_array.hpp>
#include <boost/locale.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

namespace Tools
{
	//-------------------------------------------------------------------------
	void InitConsoleAndFileLog(const boost::filesystem::path& logPath)
	{		
		boost::log::add_common_attributes();

		auto fileSink = logging::add_file_log(logPath.wstring(),
			boost::log::keywords::format =
			expr::stream
			<< "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
			<< "] [" << logging::trivial::severity
			<< "] " << expr::message);

		auto consoleSink = logging::add_console_log(std::clog,
			boost::log::keywords::format =
			expr::stream
			<< "[" << logging::trivial::severity
			<< "] " << expr::message
			);

		// Set correct endocing for special char
		auto loc = boost::locale::generator()("en_US.UTF-8");		
		fileSink->imbue(loc);
		consoleSink->imbue(loc);		
	}

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