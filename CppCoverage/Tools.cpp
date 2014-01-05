#include "stdafx.h"
#include "Tools.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	std::string Tools::ToString(const std::wstring& str)
	{
		return std::string(str.begin(), str.end());
	}

	//-------------------------------------------------------------------------
	std::wstring Tools::ToWString(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}
}


