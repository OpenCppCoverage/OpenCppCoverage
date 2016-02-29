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
#include <memory>
#include <boost/optional/optional_fwd.hpp>

namespace boost
{
	namespace filesystem
	{
		class path;
	}
}

namespace FileFilter
{
	class File;

	class FILEFILTER_DLL PathMatcher
	{
	public:
		explicit PathMatcher(
			std::vector<File>&&,
			const boost::optional<boost::filesystem::path>& parentPath);
		~PathMatcher();

		File* Match(const boost::filesystem::path&);

		using PathCollection = std::vector<boost::filesystem::path>;
		PathCollection GetUnmatchedPaths() const;

	private:
		PathMatcher(const PathMatcher&) = delete;
		PathMatcher& operator=(const PathMatcher&) = delete;
		PathMatcher(PathMatcher&&) = delete;
		PathMatcher& operator=(PathMatcher&&) = delete;

		class IPathMatcherEngine;
		class PostFixPathMatcherEngine;
		class FullPathMatcherEngine;

		std::unique_ptr<IPathMatcherEngine> pathMatcherEngine_;
	};
}

