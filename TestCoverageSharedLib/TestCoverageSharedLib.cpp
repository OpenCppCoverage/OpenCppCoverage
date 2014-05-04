// DllForCppCoverageTest.cpp : Defines the entry point for the application.
//

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
}