// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <set>
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

namespace boost
{
	namespace log
	{
		//-----------------------------------------------------------------------
		template <typename Collection>
		wformatting_ostream& LogCollection(
			wformatting_ostream& ostr, 
			const Collection& collection)
		{
			ostr << L'[';
			for (auto it = collection.begin(); it != collection.end(); ++it)
			{
				if (it != collection.begin())
					ostr << L", ";
				ostr << *it;
			}
			ostr << L']';
			return ostr;
		}

		//-----------------------------------------------------------------------
		template <typename T>
		wformatting_ostream& operator<<(
			wformatting_ostream& ostr, 
			const std::set<T>& values)
		{
			return LogCollection(ostr, values);
		}
	}
}
