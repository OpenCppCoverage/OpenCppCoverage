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
#include <boost/filesystem/path.hpp>

#pragma warning(push)
#pragma warning(disable: 4275) // warning C4275: non dll-interface class 'std::exception' used as base for dll-interface class

namespace FileFilter
{	
	class FILEFILTER_DLL AmbiguousPathException : public std::runtime_error
	{
	public:		
		AmbiguousPathException(
			const boost::filesystem::path& postFixPath,
			const boost::filesystem::path& firstPossiblePath,
			const boost::filesystem::path& secondPossiblePath);

		AmbiguousPathException(const AmbiguousPathException&) = default;

		const boost::filesystem::path& GetPostFixPath() const;
		const boost::filesystem::path& GetFirstPossiblePath() const;
		const boost::filesystem::path& GetSecondPossiblePath() const;

	private:
		boost::filesystem::path postFixPath_;
		boost::filesystem::path firstPossiblePath_;
		boost::filesystem::path secondPossiblePath_;
	};	
}

#pragma warning(pop)

