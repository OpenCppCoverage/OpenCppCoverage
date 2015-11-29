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

#include "SpecialLineInfo.hpp"
#include <string>

namespace TestCoverageConsole
{
	// This line generates compiler symbols "__global_array_delete"
	// and "__global_delete"
	template class __declspec(dllexport) std::allocator<std::string>;

	//-------------------------------------------------------------------------
	void SpecialLineInfo()
	{
		// Generate a SRCCODEINFO::LineNumber == 0x00feefee
		try
		{
			throw 42;
		}
		catch (...)
		{
		}
	}
}