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
#include <vector>
#include <boost/filesystem/path.hpp>

namespace FileFilter
{
	class FileInfo
	{
	public:
		FileInfo(
			const boost::filesystem::path& filePath,
			std::vector<LineInfo>&& lineInfoColllection)
			: filePath_{ filePath }
			, lineInfoColllection_{ std::move(lineInfoColllection) }
		{}

		const boost::filesystem::path filePath_;
		const std::vector<LineInfo> lineInfoColllection_;
	};	
}
