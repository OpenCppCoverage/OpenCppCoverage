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

#include <vector>
#include <boost/filesystem/path.hpp>

namespace FileFilter
{	
	class LineInfo
	{
	public:
		LineInfo(
			int lineNumber,
			DWORD64 lineAddress,
			ULONG symbolIndex,
			const std::wstring& symbolName)
			: lineNumber_{ lineNumber }
			, lineAddress_{ lineAddress }
			, symbolIndex_{ symbolIndex }
			, symbolName_{ symbolName }
		{}

		const int lineNumber_;
		const DWORD64 lineAddress_;
		const ULONG symbolIndex_;
		const std::wstring symbolName_;
	};
}