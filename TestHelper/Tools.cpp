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
#include <boost/filesystem/path.hpp>

//-------------------------------------------------------------------------
void TestUnloadDll()
{
}

namespace TestHelper
{
	//-------------------------------------------------------------------------
	boost::filesystem::path GetTestUnloadDllFilename()
	{
		return boost::filesystem::path(__FILE__).filename();
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path GetOutputBinaryPath()
	{		
		return TARGET_FILE_NAME;
	}

	//-------------------------------------------------------------------------
	void CreateEmptyFile(const boost::filesystem::path& path)
	{
		std::ofstream ofs;
		ofs.open(path.string(), std::ios::out);
	}
}