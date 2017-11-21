// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2017 OpenCppCoverage
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

#include <Windows.h>
#include <memory>

#include "ToolsExport.hpp"

namespace Tools
{
	//-------------------------------------------------------------------------
	TOOLS_DLL void ReadProcessMemory(HANDLE hProcess,
	                                 DWORD64 address,
	                                 void* buffer,
	                                 SIZE_T size);

	//-------------------------------------------------------------------------
	template <typename T>
	std::unique_ptr<T> ReadStructInProcessMemory(HANDLE hProcess,
	                                             DWORD64 address)
	{
		auto data = std::make_unique<T>();
		::Tools::ReadProcessMemory(hProcess, address, data.get(), sizeof(T));

		return data;
	}
}