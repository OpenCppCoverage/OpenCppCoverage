#pragma once

#include "tools/ExceptionBase.hpp"

GENERATE_EXCEPTION_CLASS(Exporter, ExporterException);

#define THROW(message) THROW_BASE(Exporter, ExporterException, message)

