#pragma once

#include <iosfwd>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#include <boost/shared_ptr.hpp>

#include "ToolsExport.hpp"

// Define the logger
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(globalLogger, boost::log::sources::wseverity_logger<boost::log::trivial::severity_level >)

#define LOG_TRACE BOOST_LOG_SEV  (globalLogger::get(), boost::log::trivial::trace) 
#define LOG_DEBUG BOOST_LOG_SEV  (globalLogger::get(), boost::log::trivial::debug) 
#define LOG_INFO BOOST_LOG_SEV   (globalLogger::get(), boost::log::trivial::info) 
#define LOG_WARNING BOOST_LOG_SEV(globalLogger::get(), boost::log::trivial::warning) 
#define LOG_ERROR BOOST_LOG_SEV  (globalLogger::get(), boost::log::trivial::error) 

namespace boost
{ 
	namespace filesystem
	{
		class path;
	}
}

namespace Tools
{
	void TOOLS_DLL InitConsoleAndFileLog(const boost::filesystem::path& logPath);
	void TOOLS_DLL SetLoggerMinSeverity(boost::log::trivial::severity_level minSeverity);
	void TOOLS_DLL EnableLogger(bool isEnabled);
	void TOOLS_DLL InitLoggerOstream(const boost::shared_ptr<std::ostringstream>& ostr);
}