#include "stdafx.h"
#include "CppCoverageException.hpp"

#include <sstream>

#include "Tools/Tool.hpp"

namespace CppCoverage
{	
	//-------------------------------------------------------------------------
	std::wstring GetErrorMessage(int lastErrorCode)
	{			
		wchar_t sysMsg[64 * 1024];
		std::wostringstream ostr;
			
		if (FormatMessage(
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
