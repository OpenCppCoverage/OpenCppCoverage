// CppConsoleApplicationDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <iostream>

__declspec(dllexport) void Hello()
{
	std::cout << "Hello!" << std::endl;
}

