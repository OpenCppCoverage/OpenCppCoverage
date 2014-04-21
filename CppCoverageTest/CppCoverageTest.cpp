// CppCoverageTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>

//-----------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}

