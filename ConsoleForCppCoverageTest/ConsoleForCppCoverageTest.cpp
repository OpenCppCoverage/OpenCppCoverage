// ConsoleForCppCoverageTest.cpp : Defines the entry point for the console application.
//

#include <windows.h>

#include <string>
#include <iostream>
#include "DllForCppCoverageTest/DllForCppCoverageTest.h"

int main(int argc, char* argv[])
{
	if (argc <= 1)	
		return 1;
	
	return Foo(std::stoul(argv[1]));
}

