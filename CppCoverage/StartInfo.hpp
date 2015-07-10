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

#include <string>
#include <vector>
#include <iosfwd>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL StartInfo
	{
	public:
		explicit StartInfo(const boost::filesystem::path&);
		
		StartInfo(StartInfo&&);

		StartInfo(const StartInfo&) = default;		
		StartInfo& operator=(const StartInfo&) = default;

		void SetWorkingDirectory(const boost::filesystem::path&);
		void AddArgument(const std::wstring&);

		const boost::filesystem::path& GetPath() const;
		const std::vector<std::wstring>& GetArguments() const;
		const boost::filesystem::path* GetWorkingDirectory() const;

		friend CPPCOVERAGE_DLL std::wostream& operator<<(std::wostream& ostr, const StartInfo&);

	private:
		boost::filesystem::path path_;
		std::vector<std::wstring> arguments_;
		boost::optional<boost::filesystem::path> workingDirectory_;
	};
}


