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

#include "stdafx.h"

#include "Plugin/Exporter/FileCoverage.hpp"
#include "CppCoverage/CppCoverageException.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	//-------------------------------------------------------------------------
	TEST(FileCoverageTest, Basic)
	{
		Plugin::FileCoverage file{ L"" };
		unsigned int lineNumber = 0;

		file.AddLine(lineNumber, true);

		auto line = file[lineNumber];
		ASSERT_NE(nullptr, line);
		ASSERT_TRUE(line->HasBeenExecuted());
		ASSERT_EQ(nullptr, file[lineNumber + 1]);	

		const auto& lines = file.GetLines();
		ASSERT_EQ(1, lines.size());
		ASSERT_EQ(line->GetLineNumber(), lines[0].GetLineNumber());
	}	

	//-------------------------------------------------------------------------
	TEST(FileCoverageTest, UpdateLine)
	{
		Plugin::FileCoverage file{ L"" };
		unsigned int lineNumber = 0;

		file.AddLine(lineNumber, true);
		file.UpdateLine(lineNumber, false);

		ASSERT_FALSE(file.GetLines().at(0).HasBeenExecuted());		
	}

	//-------------------------------------------------------------------------
	TEST(FileCoverageTest, UpdateLineNotExists)
	{
		Plugin::FileCoverage file{ L"" };
		
		ASSERT_THROW(file.UpdateLine(0, false), std::runtime_error);
	}
}