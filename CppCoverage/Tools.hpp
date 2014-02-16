#ifndef CPPCOVERAGE_TOOLS_HEADER_GARD
#define CPPCOVERAGE_TOOLS_HEADER_GARD

#include <string>

#include "Export.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL Tools
	{
	public:
		Tools() = delete;

		static std::wstring ToWString(const std::string&);
		static std::string ToString(const std::wstring&);
	};
}

#endif
