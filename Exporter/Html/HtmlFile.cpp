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
#include "HtmlFile.hpp"

namespace Exporter
{
	//-------------------------------------------------------------------------
	HtmlFile::HtmlFile(
		std::filesystem::path absolutePath,
		std::filesystem::path relativeLinkPath)
		: absolutePath_(absolutePath)
		, relativeLinkPath_(relativeLinkPath)
	{
	}

	//-------------------------------------------------------------------------
	const std::filesystem::path& HtmlFile::GetAbsolutePath() const
	{
		return absolutePath_;
	}

	//-------------------------------------------------------------------------
	const std::filesystem::path& HtmlFile::GetRelativeLinkPath() const
	{
		return relativeLinkPath_;
	}
}
