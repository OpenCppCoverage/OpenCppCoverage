#pragma once

#include "tools/ExceptionBase.hpp"

GENERATE_EXCEPTION_CLASS(CppCoverageConsole, CppCoverageConsoleException);

#define THROW(message) THROW_BASE(CppCoverageConsole, CppCoverageConsoleException, message)

