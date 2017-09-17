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
#include <Poco/Process.h>
#include <Poco/Pipe.h>
#include <Poco/PipeStream.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include "FileFilter/RelocationsExtractor.hpp"
#include "TestCoverageOptimizedBuild/TestCoverageOptimizedBuild.hpp"

namespace fs = boost::filesystem;

namespace FileFilterTest
{	
	//-------------------------------------------------------------------------
	std::string RunProcess(
		const fs::path& program, 
		const std::vector<std::string>& args)
	{
		Poco::Pipe outPipe;		
		auto handle = Poco::Process::launch(program.string(), args, nullptr, &outPipe, nullptr);
		Poco::PipeInputStream istr(outPipe);
		std::string content { std::istreambuf_iterator<char>(istr), std::istreambuf_iterator<char>() };

		int exitCode = handle.wait();
		if (exitCode)
			throw std::runtime_error("Error when running: " + program.string() + " " + content);
		return content;
	}

	//-------------------------------------------------------------------------
	fs::path GetVisualStudioPath()
	{
		fs::path programFileX86 = std::getenv("ProgramFiles(x86)");		
		auto vswhere = programFileX86 / "Microsoft Visual Studio" / "Installer" / "vswhere.exe";
				
		std::vector<std::string> args = { "-format", "value", "-property", "installationPath" };
		std::string value = RunProcess(vswhere, args);
		boost::trim(value);
		fs::path installerPath{ value };

		if (!fs::exists(installerPath))
			throw std::runtime_error("Invalid Visual Studio installation path: " + value);
		return installerPath;
	}

	//-------------------------------------------------------------------------
	fs::path GetDumpBinPath()
	{
		auto msvc = GetVisualStudioPath() / "VC" / "Tools" / "MSVC";
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
		const std::string value = RunProcess(dumpBinPath, args);
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
		const std::string value = RunProcess(dumpBinPath, args);
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
		auto relocations = extractor.Extract(hProcess, baseOfImage, baseAddress);		

		auto expectedRelocations = ExtractRelocations(dumpBinPath);
		ASSERT_EQ(relocations, expectedRelocations);
	}
}