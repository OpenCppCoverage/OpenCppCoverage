#pragma once

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

		static std::wstring GetConsoleForCppCoverageTestPath();
		static std::wstring GetConsoleForCppCoverageTestFilename();

		static void GetHandles(const std::wstring& filename, T_HandlesFct);
	};
}

