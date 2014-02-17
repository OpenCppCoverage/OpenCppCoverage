// DllForCppCoverageTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DllForCppCoverageTest.h"

#include <iostream>

int Foo(int n)
{
	if (n == 42)
		return 0;
	
	return n;
}