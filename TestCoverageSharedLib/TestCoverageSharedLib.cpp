// DllForCppCoverageTest.cpp : Defines the entry point for the application.
//

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

#include "stdafx.h"
#include "TestCoverageSharedLib.hpp"

#include <iostream>

namespace fs = boost::filesystem;

namespace TestCoverageSharedLib
{
	//-------------------------------------------------------------------------
	bool IsOdd(int n)
	{
		if (n % 2 == 0) 
			return true;

		return false;
	}

	//-------------------------------------------------------------------------
	fs::path GetMainCppPath()
	{
		return __FILE__ ;
	}

	//-------------------------------------------------------------------------
	fs::path GetOutputBinaryPath()
	{		
		return TARGET_FILE_NAME;
	}

	//-------------------------------------------------------------------------
	void CallSharedFunctionFromSharedLib()
	{
		SharedFunction(true);
	}
}