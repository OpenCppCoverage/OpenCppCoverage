#pragma once

#include "tools/ExceptionBase.hpp"

#include <string>

namespace CppCoverage
{
	std::wstring GetErrorMessage(int errorCode);
}


GENERATE_EXCEPTION_CLASS(CppCoverage, CppCoverageException);

#define THROW(message) THROW_BASE(CppCoverage, CppCoverageException, message)
#define THROW_LAST_ERROR(message, lastErrorCode) THROW_BASE(CppCoverage, CppCoverageException, message << CppCoverage::GetErrorMessage(lastErrorCode))
