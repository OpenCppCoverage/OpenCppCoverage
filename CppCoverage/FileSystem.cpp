// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2018 OpenCppCoverage
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
#include "FileSystem.hpp"
#include <boost/filesystem/operations.hpp>
#include <boost/optional/optional.hpp>

namespace CppCoverage
{
	//----------------------------------------------------------------------------
	boost::optional<std::time_t>
	FileSystem::GetLastWriteTime(const boost::filesystem::path& path) const
	{
		boost::system::error_code error;
		auto time = boost::filesystem::last_write_time(path, error);

		return error ? boost::optional<std::time_t>{} : time;
	}
}
