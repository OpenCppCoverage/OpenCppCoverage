// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <exception>
#include <string>
#include <sstream>

#include "ToolsExport.hpp"

namespace Tools
{
	class TOOLS_DLL ExceptionBase : public std::exception
	{
	protected:
		ExceptionBase(const std::wstring& message);
	};
}

#define GENERATE_EXCEPTION_CLASS(namespaceName, exceptionName)	\
namespace namespaceName											\
{																\
	class exceptionName : public Tools::ExceptionBase			\
	{															\
	public:														\
			explicit exceptionName(const std::wstring& message) \
			: ExceptionBase(message)							\
		{														\
		}														\
	};															\
}																\

#define THROW_BASE(namespaceName, className, message)		\
do															\
{															\
	std::wostringstream ostr;								\
	ostr << __FILE__ << ':' << __LINE__ << ' ' << message;	\
	throw namespaceName::className(ostr.str());				\
} while (false)												\



