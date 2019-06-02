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

#pragma once

#include "stdafx.h"
#include "Tools.hpp"
#include <fstream>
#include <filesystem>
#include <boost/algorithm/string.hpp>

#include <Poco/Process.h>
#include <Poco/Pipe.h>
#include <Poco/PipeStream.h>

namespace fs = std::filesystem;

//-------------------------------------------------------------------------
void TestUnloadDll()
{
}

namespace TestHelper
{
	//-------------------------------------------------------------------------
	fs::path GetTestUnloadDllFilename()
	{
		return fs::path(__FILE__).filename();
	}

	//-------------------------------------------------------------------------
	fs::path GetOutputBinaryPath()
	{		
		return TARGET_FILE_NAME;
	}

	//-------------------------------------------------------------------------
	void CreateEmptyFile(const fs::path& path)
	{
		std::ofstream ofs;
		ofs.open(path.string(), std::ios::out);
	}

	//-------------------------------------------------------------------------
	std::string RunProcess(const fs::path& program,
	                       const std::vector<std::string>& args)
	{
		Poco::Pipe outPipe;
		auto handle = Poco::Process::launch(
		    program.string(), args, nullptr, &outPipe, nullptr);
		Poco::PipeInputStream istr(outPipe);
		std::string content{std::istreambuf_iterator<char>(istr),
		                    std::istreambuf_iterator<char>()};

		int exitCode = handle.wait();
		if (exitCode)
			throw std::runtime_error("Error when running: " + program.string() +
			                         " " + content);
		return content;
	}

	//------------------------------------------------------------------------ -
	fs::path GetVisualStudioPath()
	{
		fs::path programFileX86 = std::getenv("ProgramFiles(x86)");
		auto vswhere = programFileX86 / "Microsoft Visual Studio" /
		               "Installer" / "vswhere.exe";

		std::vector<std::string> args = {
		    "-format", "value", "-property", "installationPath"};
		std::string value = RunProcess(vswhere, args);
		boost::trim(value);
		fs::path installerPath{value};

		if (!fs::exists(installerPath))
			throw std::runtime_error(
			    "Invalid Visual Studio installation path: " + value);
		return installerPath;
	}
}