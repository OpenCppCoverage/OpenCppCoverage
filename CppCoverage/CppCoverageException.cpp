#include "stdafx.h"
#include "CppCoverageException.hpp"

#include <sstream>

namespace CppCoverage
{
	namespace
	{
		std::string ToStdString(const std::wstring& str)
		{
			return std::string(str.begin(), str.end());
		}

		//-------------------------------------------------------------------------
		std::string WriteLastErrorMessage(const std::wstring& message, int lastErrorCode)
		{			
			char sysMsg[64 * 1024];
			std::ostringstream ostr{ ToStdString(message) };
			
			if (FormatMessageA(
					FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, lastErrorCode,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					sysMsg, sizeof(sysMsg), NULL))
			{
				ostr << sysMsg;
			}
			else
			{
				ostr << "Last error code:" << lastErrorCode;
			}

			return ostr.str();
		}

	}

	//-------------------------------------------------------------------------
	CppCoverageException::CppCoverageException(const std::wstring& message)
		: std::exception(ToStdString(message).c_str())
	{
	}
	
	//-------------------------------------------------------------------------
	CppCoverageException::CppCoverageException(const std::wstring& message, int lastErrorCode)
		: std::exception(WriteLastErrorMessage(message, lastErrorCode).c_str())
	{

	}
}
