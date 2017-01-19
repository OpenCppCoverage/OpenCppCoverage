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
#include "TestCoverageSharedLib/TestCoverageSharedLib.hpp"

namespace FileFilterTest
{	
	// These values are computed with Dumpbin /RELOCATIONS TestCoverageSharedLib.dll
#ifdef _WIN64
	#ifdef NDEBUG
		const auto firstAdress = 0x180003908;
		const auto lastAdress = 0x1800042C8;
	#else
		auto firstAdress = 0x180011E90;
		auto lastAdress = 0x180011609;
	#endif
	const auto baseAddress = 0x180000000;

#else
	#ifdef NDEBUG
		const auto firstAdress = 0x100040C4;
		const auto lastAdress = 0x10004170;
	#else
		const auto firstAdress = 0x10024190;
		const auto lastAdress = 0x10020C48;
	#endif

	const auto baseAddress = 0x10000000;
#endif

	//-------------------------------------------------------------------------
	TEST(RelocationsExtractorTest, Extract)
	{						
		FileFilter::RelocationsExtractor extractor;

		auto hProcess = GetCurrentProcess();
		auto hModule = GetModuleHandle(TestCoverageSharedLib::GetOutputBinaryPath().c_str());
		auto baseOfImage = reinterpret_cast<DWORD64>(hModule);
		ASSERT_NE(0, baseOfImage);

		auto relocations = extractor.Extract(hProcess, baseOfImage, baseAddress);		
		ASSERT_EQ(1, relocations.count(firstAdress));
		ASSERT_EQ(1, relocations.count(lastAdress));
	}
}