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
#include "FileCoverage.hpp"

#include "CppCoverageException.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	FileCoverage::FileCoverage(const boost::filesystem::path& path)
		: path_(path)
	{
	}

	//-------------------------------------------------------------------------
	void FileCoverage::AddLine(unsigned int lineNumber, bool hasBeenExecuted)
	{
		LineCoverage line{lineNumber, hasBeenExecuted};

		if (!lines_.emplace(lineNumber, line).second)
			THROW(L"Line " << lineNumber << L" already exists for " << path_.wstring());
	}

	//-------------------------------------------------------------------------
	const boost::filesystem::path& FileCoverage::GetPath() const
	{
		return path_;
	}

	//-------------------------------------------------------------------------
	const LineCoverage* FileCoverage::operator[](unsigned int line) const
	{
		auto it = lines_.find(line);

		if (it == lines_.end())
			return 0;

		return &it->second;
	}

	//-------------------------------------------------------------------------
	void FileCoverage::ComputeCoverageRate()
	{		
		int executedLines = 0;
		int unexecutedLines = 0;

		for (const auto& pair : lines_)
		{
			if (pair.second.HasBeenExecuted())
				++executedLines;
			else
				++unexecutedLines;
		}

		coverageRate_.SetExecutedLinesCount(executedLines);		
		coverageRate_.SetUnexecutedLinesCount(unexecutedLines);
	}

	//-------------------------------------------------------------------------
	const CoverageRate& FileCoverage::GetCoverageRate() const
	{
		return coverageRate_;
	}

}
