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
#include "CppCoverage/RelocationsExtractor.hpp"

#include "TestCoverageSharedLib/TestCoverageSharedLib.hpp"
#include "TestHelper/AutoClose.hpp"

using TestHelper::MakeAutoClose;

namespace CppCoverageTest
{	
	// These values are computed with Dumpbin /RELOCATIONS TestCoverageSharedLib.dll
#ifdef NDEBUG
	#ifdef _WIN64
		auto firstAdress = 0x180003908;
		auto lastAdress =  0x1800042C8;
	#else
		auto firstAdress = 0x100040BC;
		auto lastAdress =  0x10004170;
	#endif
#else
	#ifdef _WIN64
		auto firstAdress = 0x180011E60;
		auto lastAdress = 0x1800115FA;
	#else
		auto firstAdress = 0x10023180;
		auto lastAdress =  0x1001154B;
	#endif
#endif

	//-------------------------------------------------------------------------
	TEST(RelocationsExtractorTest, Extract)
	{						
		auto modulePath = TestCoverageSharedLib::GetOutputBinaryPath();
		
		auto hFile = MakeAutoClose(CreateFile(
			modulePath.wstring().c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0), INVALID_HANDLE_VALUE, CloseHandle);

		auto hFileMapping = MakeAutoClose(
			CreateFileMapping(*hFile, nullptr, PAGE_READONLY, 0, 0, nullptr), nullptr, CloseHandle);		

		auto lpFileBase = MakeAutoClose(
			MapViewOfFile(*hFileMapping, FILE_MAP_READ, 0, 0, 0), nullptr, UnmapViewOfFile);		

		auto handle = MakeAutoClose(
			LoadLibrary(modulePath.wstring().c_str()), nullptr, FreeLibrary);
				
		CppCoverage::RelocationsExtractor extractor;

		auto res = extractor.Extract(*handle, *lpFileBase);
		ASSERT_EQ(1, res.count(firstAdress));
		ASSERT_EQ(1, res.count(lastAdress));
	}
}