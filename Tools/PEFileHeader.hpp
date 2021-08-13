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

#include "ToolsExport.hpp"

#include <Windows.h>

namespace Tools
{
	class TOOLS_DLL IPEFileHeaderHandler
	{
	  public:
		virtual ~IPEFileHeaderHandler() = default;
		virtual void OnNtHeader32(HANDLE hProcess,
		                          DWORD64 baseOfImage,
		                          const IMAGE_NT_HEADERS32&) = 0;
		virtual void OnNtHeader64(HANDLE hProcess,
		                          DWORD64 baseOfImage,
		                          const IMAGE_NT_HEADERS64&) = 0;
          protected:
               void ReadFromProcessMemory(HANDLE hProcess,
                                          DWORD64 address,
                                          void* buffer,
                                          SIZE_T size);
        };

	class TOOLS_DLL PEFileHeader
	{
	  public:
		void
		Load(HANDLE hProcess, DWORD64 baseOfImage, IPEFileHeaderHandler&) const;
	};
}