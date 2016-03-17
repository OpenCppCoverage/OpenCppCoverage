// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "FileFilterExport.hpp"

#include <vector>

#include <boost/optional/optional_fwd.hpp>
#include <boost/filesystem/path.hpp>
#include "PathMatcher.hpp"

namespace FileFilter
{
	class File;

	class FILEFILTER_DLL UnifiedDiffCoverageFilter
	{
	public:
		UnifiedDiffCoverageFilter(
			const boost::filesystem::path& unifiedDiffPath,
			const boost::optional<boost::filesystem::path>& rootDiffFolder);

		UnifiedDiffCoverageFilter(
			std::vector<File>&&,
			const boost::optional<boost::filesystem::path>& rootDiffFolder);

		bool IsSourceFileSelected(const boost::filesystem::path&);
		bool IsLineSelected(const boost::filesystem::path&, int lineNumber);
		std::vector<boost::filesystem::path> GetUnmatchedPaths() const;

	private:
		UnifiedDiffCoverageFilter(const UnifiedDiffCoverageFilter&) = delete;
		UnifiedDiffCoverageFilter& operator=(const UnifiedDiffCoverageFilter&) = delete;
		UnifiedDiffCoverageFilter(UnifiedDiffCoverageFilter&&) = delete;
		UnifiedDiffCoverageFilter& operator=(UnifiedDiffCoverageFilter&&) = delete;

		File* SearchFile(const boost::filesystem::path&);

		boost::filesystem::path lastPath_;
		File* lastFile_;
		PathMatcher pathMatcher_;
	};
}

