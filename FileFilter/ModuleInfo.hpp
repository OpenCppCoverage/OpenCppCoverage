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
#include <boost/uuid/uuid.hpp>

namespace FileFilter
{
	class ModuleInfo
	{
	public:
		//---------------------------------------------------------------------------
		ModuleInfo(
				HANDLE hProcess,
				const boost::uuids::uuid& uniqueId,
				void* baseOfImage,
				DWORD64 baseAddress)
			: hProcess_ {hProcess}
			, uniqueId_{ uniqueId }
			, baseOfImage_{ baseOfImage }
			, baseAddress_{ baseAddress }
		{}

		const HANDLE hProcess_;
		const boost::uuids::uuid uniqueId_;
		void* const  baseOfImage_;
		const DWORD64 baseAddress_;
	};
}
