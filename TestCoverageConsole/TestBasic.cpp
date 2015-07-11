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

#include "TestBasic.hpp"
#include <Poco/Process.h>

#include "Tools/Tool.hpp"

#include "TestCoverageConsole.hpp"

// We cannot move this code in the header, because the tests will not work in
// release mode. (This project has no optmisation in release mode).
namespace TestCoverageConsole
{
	//-------------------------------------------------------------------------
	int GetTestBasicLine()
	{
		return __LINE__;
	}

	//-------------------------------------------------------------------------
	void TestBasic()
	{
		if (false)
		{
			int answer = 42;
		}
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path GetTestBasicPath()
	{
		return __FILE__;
	}

	//-------------------------------------------------------------------------
	int RunAsChild(int argc, _TCHAR* argv[])
	{
		auto outputBinaryPath = TestCoverageConsole::GetOutputBinaryPath();

		if (argc < 2)
			throw std::runtime_error("Invalid number of arguments.");
		if (argv[1] != TestCoverageConsole::TestChildProcess)
			throw std::runtime_error("Invalid argument.");

		std::vector<std::string> arguments;

		for (int i = 2; i < argc; ++i)
		{
			std::wstring argument = argv[i];
			arguments.push_back(Tools::ToString(argument));
		}

		auto handle = Poco::Process::launch(outputBinaryPath.string(), arguments);
		return handle.wait();
	}
}
