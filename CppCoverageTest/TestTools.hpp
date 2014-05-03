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
	class TestTools // rename
	{	
	public:
		typedef std::function<void(HANDLE hProcess, HANDLE hFile)> T_HandlesFct;

	public:
		TestTools() = delete;
	
		static void GetHandles(const boost::filesystem::path&, T_HandlesFct);
	};
}

