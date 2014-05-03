#pragma once

#include <boost/filesystem.hpp>

namespace TestCoverageSharedLib
{
	__declspec(dllexport) boost::filesystem::path GetMainCppPath();
	__declspec(dllexport) boost::filesystem::path GetTargetFileName();
	__declspec(dllexport) bool IsOdd(int n);
}
