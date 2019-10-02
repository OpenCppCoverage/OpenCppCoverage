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
#include "ModuleCoverage.hpp"

#include "FileCoverage.hpp"

namespace Plugin
{
	//-------------------------------------------------------------------------
	ModuleCoverage::ModuleCoverage(const std::filesystem::path& path)
		: path_(path)
	{
	}

	//-------------------------------------------------------------------------
	ModuleCoverage::~ModuleCoverage()
	{
	}

	//-------------------------------------------------------------------------
	FileCoverage& ModuleCoverage::AddFile(const std::filesystem::path& filePath)
	{
		files_.push_back(std::unique_ptr<FileCoverage>(new FileCoverage(filePath)));

		return *files_.back();
	}

	//-------------------------------------------------------------------------
	const std::filesystem::path& ModuleCoverage::GetPath() const
	{
		return path_;
	}

	//-------------------------------------------------------------------------
	const ModuleCoverage::T_FileCoverageCollection& ModuleCoverage::GetFiles() const
	{
		return files_;
	}
}
