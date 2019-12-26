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
#include <filesystem>

#pragma warning(push)
#pragma warning(disable: 4275) // warning C4275: non dll-interface class 'std::exception' used as base for dll-interface class

namespace FileFilter
{	
	class FILEFILTER_DLL AmbiguousPathException : public std::runtime_error
	{
	public:		
		AmbiguousPathException(
			const std::filesystem::path& postFixPath,
			const std::filesystem::path& firstPossiblePath,
			const std::filesystem::path& secondPossiblePath);

		AmbiguousPathException(const AmbiguousPathException&) = default;

		const std::filesystem::path& GetPostFixPath() const;
		const std::filesystem::path& GetFirstPossiblePath() const;
		const std::filesystem::path& GetSecondPossiblePath() const;

	private:
		std::filesystem::path postFixPath_;
		std::filesystem::path firstPossiblePath_;
		std::filesystem::path secondPossiblePath_;
	};	
}

#pragma warning(pop)

