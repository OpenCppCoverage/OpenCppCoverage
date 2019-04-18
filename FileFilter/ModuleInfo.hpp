// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage
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

#include <windows.h>
#include <filesystem>

namespace FileFilter
{
	class ModuleInfo
	{
	public:
		//---------------------------------------------------------------------------
	  ModuleInfo(HANDLE hProcess,
		         const std::filesystem::path& path,
		         void* baseOfImage)
		  : hProcess_{hProcess}, path_{path}, baseOfImage_{baseOfImage}
	  {
	  }

	  const HANDLE hProcess_;
	  const std::filesystem::path path_;
	  void* const baseOfImage_;
	};
}
