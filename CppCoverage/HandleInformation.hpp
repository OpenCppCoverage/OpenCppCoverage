#ifndef CPPCOVERAGE_HANDLEINFORMATION_HEADER_GARD
#define CPPCOVERAGE_HANDLEINFORMATION_HEADER_GARD

#include <Windows.h>

#include "Export.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL HandleInformation
	{
	public:
		HandleInformation() = default;
		std::wstring ComputeFilename(HANDLE hfile) const;

	private:
		HandleInformation(const HandleInformation&) = delete;
		HandleInformation& operator=(const HandleInformation&) = delete;
	};
}

#endif
