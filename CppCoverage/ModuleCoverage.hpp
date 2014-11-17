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

#include <vector>
#include <memory>

#include <boost/filesystem.hpp>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class FileCoverage;

	class CPPCOVERAGE_DLL ModuleCoverage
	{
	public:
		typedef std::vector<std::unique_ptr<FileCoverage>> T_FileCoverageCollection;

	public:
		explicit ModuleCoverage(const boost::filesystem::path& path);
		~ModuleCoverage();

		FileCoverage& AddFile(const boost::filesystem::path& filename);
		
		const boost::filesystem::path& GetPath() const;
		const T_FileCoverageCollection& GetFiles() const;

	private:
		ModuleCoverage(const ModuleCoverage&) = delete;
		ModuleCoverage& operator=(const ModuleCoverage&) = delete;
		
	private:
		T_FileCoverageCollection files_;
		boost::filesystem::path path_;		
	};
}


