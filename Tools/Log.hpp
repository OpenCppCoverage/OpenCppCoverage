#pragma once

#include <boost/log/trivial.hpp>

#define LOG_TRACE BOOST_LOG_TRIVIAL(trace) 
#define LOG_DEBUG BOOST_LOG_TRIVIAL(debug) 
#define LOG_INFO BOOST_LOG_TRIVIAL(info) 
#define LOG_WARNING BOOST_LOG_TRIVIAL(warning) 
#define LOG_ERROR BOOST_LOG_TRIVIAL(error) 