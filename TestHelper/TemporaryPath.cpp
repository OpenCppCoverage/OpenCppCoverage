// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
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
#include "TemporaryPath.hpp"

#include "Tools/Log.hpp"

namespace fs = boost::filesystem;

namespace TestHelper
{
	//-------------------------------------------------------------------------	
	TemporaryPath::TemporaryPath(bool createPath)
	{
		path_ = fs::absolute(fs::temp_directory_path() / fs::unique_path());

		if (createPath)
			fs::create_directories(path_);
	}

	//-------------------------------------------------------------------------
	TemporaryPath::~TemporaryPath()
	{
		boost::system::error_code error;
		if (!fs::remove_all(path_, error))
		{
			LOG_ERROR << error;
		}
	}

	//-------------------------------------------------------------------------
	TemporaryPath::operator const boost::filesystem::path& () const
	{
		return GetPath();
	}

	//-------------------------------------------------------------------------
	const boost::filesystem::path& TemporaryPath::GetPath() const
	{
		return path_;
	}
}
