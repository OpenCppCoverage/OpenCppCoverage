#ifndef CPPCOVERAGE_CPPCOVERAGEEXCEPTION_HEADER_GARD
#define CPPCOVERAGE_CPPCOVERAGEEXCEPTION_HEADER_GARD

#include <exception>
#include <string>
#include <sstream>

namespace CppCoverage
{
	class CppCoverageException: public std::exception
	{
	public:
		CppCoverageException(const std::wstring& message);
		CppCoverageException(const std::wstring& message, int lastErrorCode);	
	};
}

#define THROW(message)										\
do															\
{															\
	std::wostringstream ostr;								\
	ostr << __FILE__ << ':' << __LINE__ << ' ' << message;	\
	throw CppCoverageException(ostr.str());					\
} while (false)												\


#define THROW_LAST_ERROR(message, lastErrorCode)			\
do															\
{															\
	std::wostringstream ostr;								\
	ostr << __FILE__ << ':' << __LINE__ << ' ' << message;	\
	throw CppCoverageException(ostr.str(), lastErrorCode);	\
} while (false)												\

#endif
