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

#include "stdafx.h"
#include "UniquePath.hpp"

#include <boost/filesystem/path.hpp>

namespace Tools
{
	//-------------------------------------------------------------------------
	boost::filesystem::path UniquePath::GetUniquePath(const boost::filesystem::path& path)
	{
		auto uniquePath = path;
		auto filenameWithoutExtension = path.filename().replace_extension(L"");

		for (int i = 2; existingPathSet_.count(uniquePath) != 0; ++i)
		{
			auto filename = filenameWithoutExtension.wstring() + std::to_wstring(i) + path.extension().wstring();
			uniquePath = path.parent_path() / filename;
		}
		existingPathSet_.insert(uniquePath);
		return uniquePath;
	}
}
