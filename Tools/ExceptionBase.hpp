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
	class exceptionName : public ::Tools::ExceptionBase			\
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



