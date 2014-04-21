#pragma once

#include "Tools/ExceptionBase.hpp"

GENERATE_EXCEPTION_CLASS(Tools, ToolsException);

#define THROW(message) THROW_BASE(Tools, ToolsException, message)

