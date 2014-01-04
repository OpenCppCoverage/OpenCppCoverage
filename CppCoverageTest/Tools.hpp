#ifndef CPPCOVERAGETEST_TOOLS_HEADER_GARD
#define CPPCOVERAGETEST_TOOLS_HEADER_GARD

#include <Windows.h>

#include <string>
#include <functional>

namespace CppCoverageTest
{
	class Tools
	{	
	public:
		typedef std::function<void(HANDLE hProcess, HANDLE hFile)> T_HandlesFct;

	public:
		Tools() = delete;

		static std::wstring GetConsoleForCppCoverageTest();
	
		static void GetHandles(const std::wstring& filename, T_HandlesFct);
	};
}

#endif