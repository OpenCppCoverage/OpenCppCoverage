// ConsoleForCppCoverageTest.cpp : Defines the entry point for the console application.
//

#include <windows.h>

#include <string>
#include <iostream>
#include "DllForCppCoverageTest/DllForCppCoverageTest.h"

void ThrowHandledException()
{
	try
	{
		throw 42;
	}
	catch (...)
	{
	}
}

int main(int argc, char* argv[])
{	
	if (argc <= 1)	
		return 1;
	
	if (argc == 2)
	{
		std::string argument = argv[1];
		if (argument == "ThrowHandledException")		
			ThrowHandledException();
		else if (argument == "ThrowUnHandledException")
			throw 42;
		else
			return Foo(std::stoul(argv[1]));
	}
	return 0;	
}

