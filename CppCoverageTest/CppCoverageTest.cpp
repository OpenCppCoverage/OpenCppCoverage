// CppCoverageTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>

#include "CppCoverage/CppCoverageLog.hpp"

int _tmain(int argc, _TCHAR* argv[])
{
	CppCoverage::ResetLogger();
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}

