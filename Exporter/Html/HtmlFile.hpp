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

#pragma once

#include <boost/filesystem.hpp>

#include "../ExporterExport.hpp"

namespace Exporter
{
	class EXPORTER_DLL HtmlFile
	{
	public:
		HtmlFile(
			boost::filesystem::path absolutePath,
			boost::filesystem::path relativeLinkPath);
		HtmlFile(const HtmlFile&) = default;

		const boost::filesystem::path& GetAbsolutePath() const;
		const boost::filesystem::path& GetRelativeLinkPath() const;

	private:		
		HtmlFile& operator=(const HtmlFile&) = delete;

		boost::filesystem::path absolutePath_;
		boost::filesystem::path relativeLinkPath_;
	};
}

