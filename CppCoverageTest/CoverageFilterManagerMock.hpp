// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage
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

#include <gmock/gmock.h>
#include "CppCoverage/ICoverageFilterManager.hpp"

namespace CppCoverageTest
{
	class CoverageFilterManagerMock : public CppCoverage::ICoverageFilterManager
	{
	public:
		CoverageFilterManagerMock() = default;

		MOCK_CONST_METHOD1(IsModuleSelected, bool(const std::wstring&));
		MOCK_CONST_METHOD1(IsSourceFileSelected, bool(const std::wstring&));

	private:
		CoverageFilterManagerMock(const CoverageFilterManagerMock&) = delete;
		CoverageFilterManagerMock& operator=(const CoverageFilterManagerMock&) = delete;
	};
}


