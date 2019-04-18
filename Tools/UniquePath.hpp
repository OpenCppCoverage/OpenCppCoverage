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

#include "ToolsExport.hpp"

#include <set>
#include <filesystem>

namespace Tools
{
	class TOOLS_DLL UniquePath
	{
	public:
		UniquePath() = default;

		UniquePath(const UniquePath&) = delete;
		UniquePath& operator=(const UniquePath&) = delete;
		UniquePath(UniquePath&&) = delete;
		UniquePath& operator=(UniquePath&&) = delete;

		// GetUniquePath returns "path" if there was no previous 
		// call to GetUniquePath with this value else it returns an unique
		// path starting with "path".
		std::filesystem::path GetUniquePath(const std::filesystem::path& path);

	private:
		std::set<std::filesystem::path> existingPathSet_;
	};
}
