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

#include "TestHelperExport.hpp"

namespace boost
{
	namespace filesystem
	{
		class path;
	}
}

extern "C"
{
	void TEST_HELPER_DLL TestUnloadDll();
}

namespace TestHelper
{
	boost::filesystem::path TEST_HELPER_DLL  GetTestUnloadDllFilename();
	boost::filesystem::path TEST_HELPER_DLL  GetOutputBinaryPath();

	void TEST_HELPER_DLL CreateEmptyFile(const boost::filesystem::path&);

	std::string TEST_HELPER_DLL RunProcess(
		const boost::filesystem::path& program,
		const std::vector<std::string>& args);

	boost::filesystem::path TEST_HELPER_DLL GetVisualStudioPath();

	//-------------------------------------------------------------------------
	template <typename ExceptionType, typename Fct>
	void AssertThrow(Fct fct,
	                 std::function<void(const ExceptionType&)> exceptionCheck)
	{
		try
		{
			fct();
		}
		catch (const ExceptionType& e)
		{
			exceptionCheck(e);
			return;
		}
		catch (...)
		{
			throw std::runtime_error("Expected exception not raised.");
		}
		throw std::runtime_error("No exception raised.");
	}
}
