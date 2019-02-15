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

#include "TestCoverageOptimizedBuild.hpp"

#include <memory>

namespace TestCoverageOptimizedBuild
{
	//-------------------------------------------------------------------------
	boost::filesystem::path GetMainCppPath()
	{
		return __FILE__;
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path GetOutputBinaryPath()
	{
		return boost::filesystem::path(OUT_DIR) / TARGET_FILE_NAME;
	}

	//-------------------------------------------------------------------------
	class CrashInOptimizedBuild
	{
	public:
		explicit CrashInOptimizedBuild(int type)
		{
			switch (type)
			{
			case 0: x = 1; break;
			case 1: x = 2; break;
			case 2: x = 3; break;
			case 3: x = 4; break;
			default: throw std::runtime_error("Error");
			};
		}

		int x;
	};

	//-------------------------------------------------------------------------
	void TestOptimizedBuild()
	{
		auto crashInOptimizedBuild = std::make_unique<CrashInOptimizedBuild>(0);
	}
}