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

#pragma once

#include <string>

namespace boost
{
	namespace filesystem
	{
		class path;
	}
}

namespace TestCoverageConsole
{
	//-------------------------------------------------------------------------
	inline boost::filesystem::path GetMainCppPath()
	{
		return boost::filesystem::path(__FILE__).replace_extension("cpp");
	}

	//-------------------------------------------------------------------------
	inline boost::filesystem::path GetMainCppFilename()
	{
		return GetMainCppPath().filename();
	}

	//-------------------------------------------------------------------------
	inline boost::filesystem::path GetOutputBinaryPath()
	{
		return boost::filesystem::path(OUT_DIR) / "TestCoverageConsole.exe";
	}

	//-------------------------------------------------------------------------
	inline int GetTestCoverageConsoleCppMainStartLine()
	{
		return 56;
	}

	//-------------------------------------------------------------------------
	inline int GetTestCoverageConsoleCppMainReturnLine()
	{
		return GetTestCoverageConsoleCppMainStartLine() + 39;
	}

	const std::wstring TestBasic = L"TestBasic";
	const std::wstring TestThread = L"TestThread";
	const std::wstring TestSharedLib = L"TestSharedLib";
	const std::wstring TestThrowHandledException = L"TestThrowHandledException";
	const std::wstring TestThrowUnHandledCppException = L"TestThrowUnHandledCppException";
	const std::wstring TestThrowUnHandledSEHException = L"TestThrowUnHandledSEHException";
	const std::wstring TestBreakPoint = L"TestBreakPoint";
	const std::wstring TestChildProcess = L"ChildProcess";
	const std::wstring TestFileInSeveralModules = L"FileInSeveralModules";
	const std::wstring TestSpecialLineInfo = L"TestSpecialLineInfo";
	const std::wstring TestUnloadReloadDll = L"TestUnloadReloadDll";
	const std::wstring TestDiff = L"TestDiff";
	const std::wstring TestOptimizedBuild = L"TestOptimizedBuild";
}
