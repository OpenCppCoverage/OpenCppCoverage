// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2018 OpenCppCoverage
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

#include "CppCoverage/IFileSystem.hpp"

namespace CppCoverageTest
{
	class FileSystemMock : public CppCoverage::IFileSystem
	{
	  public:
		MOCK_CONST_METHOD1(
		    GetLastWriteTime,
		    boost::optional<std::filesystem::file_time_type>(const std::filesystem::path&));
	};
}