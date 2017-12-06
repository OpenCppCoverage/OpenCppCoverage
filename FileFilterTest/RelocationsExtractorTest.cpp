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
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include "FileFilter/RelocationsExtractor.hpp"
#include "TestCoverageOptimizedBuild/TestCoverageOptimizedBuild.hpp"

#include "TestHelper/Tools.hpp"

namespace fs = boost::filesystem;

namespace FileFilterTest
{	
	//-------------------------------------------------------------------------
	fs::path GetDumpBinPath()
	{
		auto msvc = TestHelper::GetVisualStudioPath() / "VC" / "Tools" / "MSVC";
		fs::recursive_directory_iterator end;

		const auto it = std::find_if(fs::recursive_directory_iterator(msvc), end,
			[](const fs::directory_entry& entry) {
			return boost::iequals(entry.path().filename().string(), "dumpbin.exe");
		});
		if (it == end)
			throw std::runtime_error("Cannot find dumpbin.exe");
		return it->path();
	}
	
	//-------------------------------------------------------------------------
	DWORD64 ToDWord64(const std::string& str)
	{
		return strtoll(str.c_str(), nullptr, 16);
	}

	//-------------------------------------------------------------------------
	std::unordered_set<DWORD64> ExtractRelocations(const fs::path& dumpBinPath)
	{		
		std::vector<std::string> args = { 
			"/RELOCATIONS", 
			TestCoverageOptimizedBuild::GetOutputBinaryPath().string() };
		const std::string value = TestHelper::RunProcess(dumpBinPath, args);
		auto current = value.begin();
		auto end = value.end();
		std::smatch match;
		std::unordered_set<DWORD64> relocations;

		// Example: "      CCF  HIGHLOW            1001C3E8"
		//       or "      150  DIR64      000000018001B7D0"
		std::regex r(R"(^\s*[[:xdigit:]]*\s*(HIGHLOW|DIR64)\s*([[:xdigit:]]*))");

		while (std::regex_search(current, end, match, r))
		{			
			if (match.size() == 3)
			{
				auto addressStr = match[2];
				auto address = ToDWord64(addressStr);
				relocations.insert(address);
			}
			current += match.position() + match.length();
		}
		return relocations;
	}

	//-------------------------------------------------------------------------
	DWORD64 ExtractBaseAddress(const fs::path& dumpBinPath)
	{		
		std::vector<std::string> args = { 
			"/HEADERS", 
			TestCoverageOptimizedBuild::GetOutputBinaryPath().string() };
		const std::string value = TestHelper::RunProcess(dumpBinPath, args);
		std::smatch match;
		
		// Example: "        10000000 image base (10000000 to 1001FFFF)"		
		std::regex r(R"(^\s*([[:xdigit:]]*) image base )");

		if (!std::regex_search(value, match, r) || match.size() != 2)
			throw std::runtime_error("Cannot extract image base");
				
		auto addressStr = match[1];
		return ToDWord64(addressStr);
	}

	//-------------------------------------------------------------------------
	TEST(RelocationsExtractorTest, Extract)
	{			
		FileFilter::RelocationsExtractor extractor;

		auto hProcess = GetCurrentProcess();
		auto hModule = GetModuleHandle(TestCoverageOptimizedBuild::GetOutputBinaryPath().c_str());
		auto baseOfImage = reinterpret_cast<DWORD64>(hModule);
		ASSERT_NE(0, baseOfImage);

		auto dumpBinPath = GetDumpBinPath();
		auto baseAddress = ExtractBaseAddress(dumpBinPath);
		auto relocations = extractor.Extract(hProcess, baseOfImage);

		std::unordered_set<DWORD64> relocationsWithBaseAddress;
		for (auto relocation : relocations)
			relocationsWithBaseAddress.insert(relocation + baseAddress);
		auto expectedRelocations = ExtractRelocations(dumpBinPath);
		ASSERT_EQ(relocationsWithBaseAddress, expectedRelocations);
	}
}