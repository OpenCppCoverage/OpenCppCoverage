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

#include <filesystem>

#include "TestHelperExport.hpp"

namespace TestHelper
{
	enum class TemporaryPathOption
	{
		DoNotCreate,
		CreateAsFile,
		CreateAsFolder
	};

	class TEST_HELPER_DLL TemporaryPath
	{
	public:
		 
		explicit TemporaryPath(TemporaryPathOption = TemporaryPathOption::DoNotCreate);
		~TemporaryPath();

		operator const std::filesystem::path& () const;
		const std::filesystem::path& GetPath() const;
		const std::filesystem::path* operator->() const;

	private:
		TemporaryPath(const TemporaryPath&) = delete;
		TemporaryPath& operator=(const TemporaryPath&) = delete;

	private:
		std::filesystem::path path_;
	};
}