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

#include "stdafx.h"
#include "ProcessMemory.hpp"
#include "ToolsException.hpp"
#include "Log.hpp"

namespace Tools
{
	//-------------------------------------------------------------------------
	std::vector<unsigned char>
	ReadProcessMemory(HANDLE hProcess, void* address, size_t size)
	{
		std::vector<unsigned char> data(size);
		ReadProcessMemory(hProcess,
		                  reinterpret_cast<DWORD64>(address),
		                  &data[0],
		                  data.size());
		return data;
	}

	//-------------------------------------------------------------------------
	void ReadProcessMemory(HANDLE hProcess,
	                       DWORD64 address,
	                       void* buffer,
	                       SIZE_T size)
	{
		SIZE_T totalBytesRead = 0;
		SIZE_T bytesRead = 0;

		while (totalBytesRead < size)
		{
			if (!::ReadProcessMemory(
			        hProcess,
			        reinterpret_cast<void*>(address),
			        &reinterpret_cast<char*>(buffer)[totalBytesRead],
			        size - totalBytesRead,
			        &bytesRead))
			{
				LOG_ERROR << "Cannot read memory";
			}
			if (bytesRead == 0)
				THROW("Cannot ready process memory");

			totalBytesRead += bytesRead;
		}
	}
}
