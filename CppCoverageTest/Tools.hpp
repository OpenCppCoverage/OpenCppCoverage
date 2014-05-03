#pragma once

#include <Windows.h>

#include <string>
#include <functional>

namespace boost
{
	namespace filesystem
	{
		class path;
	}
}

namespace CppCoverageTest
{
	class Tools // rename
	{	
	public:
		typedef std::function<void(HANDLE hProcess, HANDLE hFile)> T_HandlesFct;

	public:
		Tools() = delete;
	
		static void GetHandles(const boost::filesystem::path&, T_HandlesFct);
	};
}

