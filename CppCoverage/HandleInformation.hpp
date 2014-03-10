#pragma once

#include <Windows.h>

#include "CppCoverageExport.hpp"

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


