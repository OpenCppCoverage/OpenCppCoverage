#include "stdafx.h"
#include "Log.hpp"

#include <boost/log/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/shared_array.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

#include <boost/locale.hpp> // $$
namespace src = boost::log::sources;
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

std::locale loc;
std::locale loc2;

namespace Tools
{
	//-------------------------------------------------------------------------
	void InitConsoleAndFileLog()
	{
		auto fileSink = logging::add_file_log("Test.log",
			boost::log::keywords::format =
			expr::stream
			<< "[" << logging::trivial::severity
			<< "] " << expr::message);

		auto consoleSink = logging::add_console_log(std::clog,
			boost::log::keywords::format =
			expr::stream
			<< "[" << logging::trivial::severity
			<< "] " << expr::message
			);

		loc = boost::locale::generator()("en_US.UTF-8");
		loc2 = boost::locale::generator()("en_US.UTF-8");
		fileSink->imbue(loc);
		consoleSink->imbue(loc2);

		// National characters are also supported
		const wchar_t national_chars[] = { 0x041f, 0x0440, 0x0438, 0x0432, 0x0435, 0x0442, L',', L' ', 0x043c, 0x0438, 0x0440, L'!', 0 };

		//BOOST_LOG_SEV(slg, ::boost::log::trivial::normal) << L"A normal severity message, will not pass to the file";
		LOG_ERROR << L"A warning severity message, will pass to the file111111111111111111111111111111111111";
		LOG_ERROR << L"An error severity message, will pass to the file";
		LOG_ERROR << national_chars;
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