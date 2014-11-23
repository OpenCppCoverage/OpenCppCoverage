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

namespace ExporterTest
{
	//---------------------------------------------------------------------
	template<typename Container, typename CompareFct>
	void AssertContainerEqual(
		const Container& container1,
		const Container& container2,
		const CompareFct& compareFct)
	{
		ASSERT_EQ(container1.size(), container2.size());

		for (size_t i = 0; i < container1.size(); ++i)
		{
			const auto& value1 = container1[i];
			const auto& value2 = container2[i];

			compareFct(value1, value2);
		}
	}

	//---------------------------------------------------------------------
	template<typename Container, typename CompareFct>
	void AssertContainerUniquePtrEqual(
		const Container& container1,
		const Container& container2,
		const CompareFct& compareFct)
	{
		using ValueType = Container::value_type;
		AssertContainerEqual(container1, container2,
			[&](const ValueType& value1, const ValueType& value2)
		{
			ASSERT_TRUE(value1);
			ASSERT_TRUE(value2);
			compareFct(*value1, *value2);
		});
	}
}