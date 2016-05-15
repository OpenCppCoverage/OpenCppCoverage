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

#include "stdafx.h"
#include "Tools/Tool.hpp"

namespace ToolsTests
{
	//---------------------------------------------------------------------
	TEST(Tool, ToString)
	{
		ASSERT_EQ("123456789", Tools::ToString(L"123456789"));
		ASSERT_EQ(1, Tools::ToString(L"1").size());
		ASSERT_EQ(std::string("יאט"), Tools::ToString(L"יאט"));
	}

	//---------------------------------------------------------------------
	TEST(Tool, ToWString)
	{
		ASSERT_EQ(L"123456789", Tools::ToWString("123456789"));
		ASSERT_EQ(1, Tools::ToWString("1").size());
		ASSERT_EQ(std::wstring(L"יאט"), Tools::ToWString("יאט"));
	}
}