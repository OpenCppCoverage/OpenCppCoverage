// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <windows.h>
#include <tchar.h>

#include <string>
#include <iostream>

#include "TestCoverageSharedLib/TestCoverageSharedLib.hpp"

#include "TestCoverageConsole.hpp"
#include "TestBasic.hpp"
#include "TestThread.hpp"

namespace
{
	//-----------------------------------------------------------------------------
	void ThrowHandledException()
	{
		try
		{
			throw 42;
		}
		catch (...)
		{
		}
	}	
}

//-----------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{	
	if (argc != 2)
		std::wcout << L"Invalid number of argument. Expect 2 instead of " << argc << std::endl;
	else
	{
		std::wstring type = argv[1];

		if (type == TestCoverageConsole::GetTestBasicPath())
			TestCoverageConsole::TestBasic();
		else if (type == TestCoverageConsole::GetTestThreadPath())
			TestCoverageConsole::RunThread();
		else if (type == TestCoverageSharedLib::GetMainCppPath())
			TestCoverageSharedLib::IsOdd(42);
		else if (type == TestCoverageConsole::TestThrowHandledException)
			ThrowHandledException();
		else if (type == TestCoverageConsole::TestThrowUnHandledCppException)
			throw 42;
		else if (type == TestCoverageConsole::TestThrowUnHandledSEHException)
			*reinterpret_cast<int*>(0) = 42;
		else
			std::wcerr << L"Unsupported type:" << type << std::endl;
	}
	return 0;	
}

namespace TestCoverageConsole
{	
	//-----------------------------------------------------------------------------
	boost::filesystem::path GetMainCppPath()
	{
		return __FILE__;
	}
	
	//-----------------------------------------------------------------------------
	boost::filesystem::path GetOutputBinaryPath()
	{
		return TARGET_PATH;
	}
}