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
#include "AmbiguousPathException.hpp"

namespace FileFilter
{	
	//-------------------------------------------------------------------------
	AmbiguousPathException::AmbiguousPathException(
		const std::filesystem::path& postFixPath,
		const std::filesystem::path& firstPossiblePath,
		const std::filesystem::path& secondPossiblePath)
		: std::runtime_error{"AmbiguousPathException"}
		, postFixPath_{ postFixPath }
		, firstPossiblePath_{ firstPossiblePath }
		, secondPossiblePath_{ secondPossiblePath }
	{
	}

	//-------------------------------------------------------------------------
	const std::filesystem::path& AmbiguousPathException::GetPostFixPath() const
	{
		return postFixPath_;
	}

	//-------------------------------------------------------------------------
	const std::filesystem::path& AmbiguousPathException::GetFirstPossiblePath() const
	{
		return firstPossiblePath_;
	}

	//-------------------------------------------------------------------------
	const std::filesystem::path& AmbiguousPathException::GetSecondPossiblePath() const
	{
		return secondPossiblePath_;
	}
}
