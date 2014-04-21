#include "stdafx.h"
#include "ExceptionBase.hpp"
#include "Tool.hpp"

namespace Tools
{
	//-------------------------------------------------------------------------
	ExceptionBase::ExceptionBase(const std::wstring& message)
		: std::exception(ToString(message).c_str())
	{
	}
}