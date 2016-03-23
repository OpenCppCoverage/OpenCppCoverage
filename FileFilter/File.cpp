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
#include "File.hpp"

namespace FileFilter
{
	//----------------------------------------------------------------------------
	File::File(const boost::filesystem::path& path)
		: path_{path}
	{
	}

	//----------------------------------------------------------------------------
	void File::AddSelectedLines(const std::vector<int>& lines)
	{
		selectedLines_.insert(lines.begin(), lines.end());
	}

	//----------------------------------------------------------------------------
	const boost::filesystem::path& File::GetPath() const
	{
		return path_;
	}

	//----------------------------------------------------------------------------
	void File::SetPath(const boost::filesystem::path& path)
	{
		path_ = path;
	}

	//----------------------------------------------------------------------------
	const std::set<int>& File::GetSelectedLines() const
	{
		return selectedLines_;
	}

	//----------------------------------------------------------------------------
	bool File::IsLineSelected(int lineNumber) const
	{
		return selectedLines_.find(lineNumber) != selectedLines_.end();
	}

	//----------------------------------------------------------------------------
	bool File::operator==(const File& other) const
	{
		return path_ == other.path_
			&& selectedLines_ == other.selectedLines_;
	}
}
