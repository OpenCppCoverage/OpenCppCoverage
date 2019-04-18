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
#include <system_error>
#include <random>

#include "TemporaryPath.hpp"

#include "Tools/Log.hpp"
#include "TestHelper/Tools.hpp"

namespace fs = std::filesystem;

namespace TestHelper
{
	//-------------------------------------------------------------------------	
	TemporaryPath::TemporaryPath(TemporaryPathOption temporaryPathOption)
	{
		auto now = std::chrono::system_clock::now().time_since_epoch();
		auto seed = static_cast<unsigned int>(now.count());
		std::default_random_engine generator(seed);
		std::uniform_int_distribution<int> distribution;
		auto temp_directory = fs::temp_directory_path();

		do
		{
			path_ = fs::absolute(temp_directory / std::to_string(distribution(generator)));
		} while (fs::exists(path_));

		if (temporaryPathOption == TemporaryPathOption::CreateAsFolder)
			fs::create_directories(path_);
		else if (temporaryPathOption == TemporaryPathOption::CreateAsFile)		
			CreateEmptyFile(path_);
	}

	//-------------------------------------------------------------------------
	TemporaryPath::~TemporaryPath()
	{
		std::error_code error;
		if (!fs::remove_all(path_, error))
		{
			LOG_ERROR << error;
		}
	}

	//-------------------------------------------------------------------------
	TemporaryPath::operator const std::filesystem::path& () const
	{
		return GetPath();
	}

	//-------------------------------------------------------------------------
	const std::filesystem::path& TemporaryPath::GetPath() const
	{
		return path_;
	}

	//-------------------------------------------------------------------------
	const std::filesystem::path* TemporaryPath::operator->() const
	{
		return &path_;
	}
}
