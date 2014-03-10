#include "stdafx.h"
#include "Tool.hpp"

namespace Tools
{
	//-------------------------------------------------------------------------
	std::string Tool::ToString(const std::wstring& str)
	{
		return std::string(str.begin(), str.end());
	}

	//-------------------------------------------------------------------------
	std::wstring Tool::ToWString(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}
}


