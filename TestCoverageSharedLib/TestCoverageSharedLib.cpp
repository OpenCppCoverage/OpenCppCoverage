// DllForCppCoverageTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TestCoverageSharedLib.h"

#include <iostream>

namespace TestCoverageSharedLib
{
	//-------------------------------------------------------------------------
	bool IsOdd(int n)
	{
		if (n % 2 == 0) 
			return true;

		return false;
	}
}