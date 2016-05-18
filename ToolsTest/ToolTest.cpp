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
	TEST(Tool, ToLocalString)
	{
		ASSERT_EQ("", Tools::ToLocalString(L""));
		ASSERT_EQ("123456789", Tools::ToLocalString(L"123456789"));
		ASSERT_EQ(1, Tools::ToLocalString(L"1").size());
		ASSERT_EQ(std::string("יאט"), Tools::ToLocalString(L"יאט"));
	}

	//---------------------------------------------------------------------
	TEST(Tool, LocalToWString)
	{
		ASSERT_EQ(L"", Tools::LocalToWString(""));
		ASSERT_EQ(L"123456789", Tools::LocalToWString("123456789"));
		ASSERT_EQ(1, Tools::LocalToWString("1").size());
		ASSERT_EQ(std::wstring(L"יאט"), Tools::LocalToWString("יאט"));
	}

	//---------------------------------------------------------------------
	TEST(Tool, Uft8)
	{
		ASSERT_EQ(L"יאט", Tools::Utf8ToWString(Tools::ToUtf8String(L"יאט")));
	}
}