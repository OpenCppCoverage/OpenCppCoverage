// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage
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

#include "stdafx.h"
#include <windows.h>

#include "FileFilter/RelocationsExtractor.hpp"
#include "TestCoverageOptimizedBuild/TestCoverageOptimizedBuild.hpp"

namespace FileFilterTest
{	
	// These values are computed with Dumpbin /RELOCATIONS TestCoverageOptimizedBuild.dll
#ifdef _WIN64
	#ifdef NDEBUG
		const auto firstAddress = 0x180002CDC;
		const auto lastAddress = 0x180003228;
	#else
		auto firstAddress = 0x180011790;

		// This the second to last address because the last one change 
		// from one computer to another.
		auto lastAddress = 0x18001102D; 
	#endif
	const auto baseAddress = 0x180000000;

#else
	#ifdef NDEBUG
		const auto firstAddress = 0x100030C4;
		const auto lastAddress = 0x10003104;
	#else
		const auto firstAddress = 0x1001D158;
		const auto lastAddress = 0x100112B2;
	#endif

	const auto baseAddress = 0x10000000;
#endif

	//-------------------------------------------------------------------------
	TEST(RelocationsExtractorTest, Extract)
	{						
		FileFilter::RelocationsExtractor extractor;

		auto hProcess = GetCurrentProcess();
		auto hModule = GetModuleHandle(TestCoverageOptimizedBuild::GetOutputBinaryPath().c_str());
		auto baseOfImage = reinterpret_cast<DWORD64>(hModule);
		ASSERT_NE(0, baseOfImage);

		auto relocations = extractor.Extract(hProcess, baseOfImage, baseAddress);		
		ASSERT_EQ(1, relocations.count(firstAddress));
		ASSERT_EQ(1, relocations.count(lastAddress));
	}
}