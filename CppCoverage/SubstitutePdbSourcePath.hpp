// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2017 OpenCppCoverage
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

#include <boost/filesystem/path.hpp>
#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL SubstitutePdbSourcePath
	{
	  public:
		//---------------------------------------------------------------------
		SubstitutePdbSourcePath(
		    boost::filesystem::path&& pdbStartPath,
		    boost::filesystem::path&& localPath)
		    : pdbStartPath_{std::move(pdbStartPath)}, localPath_{std::move(localPath)}
		{
		}

		//---------------------------------------------------------------------
		SubstitutePdbSourcePath(SubstitutePdbSourcePath&) = default;
		SubstitutePdbSourcePath(SubstitutePdbSourcePath&&) = default;

		//---------------------------------------------------------------------
		const boost::filesystem::path& GetPdbStartPath() const
		{
			return pdbStartPath_;
		}

		//---------------------------------------------------------------------
		const boost::filesystem::path& GetLocalPath() const
		{
			return localPath_;
		}

	  private:
		boost::filesystem::path pdbStartPath_;
		boost::filesystem::path localPath_;
	};
}
