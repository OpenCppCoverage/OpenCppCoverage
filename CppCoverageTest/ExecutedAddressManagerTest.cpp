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

#include "CppCoverage/ExecutedAddressManager.hpp"
#include "CppCoverage/CppCoverageException.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	//-------------------------------------------------------------------------
	TEST(ExecutedAddressManagerTest, RegisterAddress)
	{		
		cov::ExecutedAddressManager manager;
		
		ASSERT_THROW(manager.RegisterAddress(0, L"", 0, 0), cov::CppCoverageException);

		manager.SetCurrentModule(L"");
		manager.RegisterAddress(0, L"", 0, 0);
	}

	//-------------------------------------------------------------------------
	TEST(ExecutedAddressManagerTest, MarkAddressAsExecuted)
	{
		cov::ExecutedAddressManager manager;

		manager.SetCurrentModule(L"");

		ASSERT_EQ(boost::none, manager.MarkAddressAsExecuted(0));

		manager.RegisterAddress(0, L"", 0, 0);
		ASSERT_NO_THROW(manager.MarkAddressAsExecuted(0));
	}	

	//-------------------------------------------------------------------------
	TEST(ExecutedAddressManagerTest, CreateCoverageData)
	{
		cov::ExecutedAddressManager manager;
		
		const std::wstring moduleName = L"module";
		const std::wstring filename = L"filename";
		const char instructionLine42 = 10;
		const char instructionLine43 = 11;
		auto address1 = reinterpret_cast<void*>(1);
		auto address2 = reinterpret_cast<void*>(2);

		manager.SetCurrentModule(moduleName);
		manager.RegisterAddress(address1, filename, 42, instructionLine42);
		manager.RegisterAddress(address2, filename, 43, instructionLine43);
		manager.MarkAddressAsExecuted(address2);

		const cov::CoverageData coverageData = manager.CreateCoverageData(L"", 0);
		
		const auto& modules = coverageData.GetModules();
		ASSERT_EQ(1, modules.size());

		const auto& module = *modules.front();
		ASSERT_EQ(moduleName, module.GetPath());

		const auto& files = module.GetFiles();
		ASSERT_EQ(1, files.size());

		const auto& file = *files.front();
		ASSERT_EQ(filename, file.GetPath());

		const auto* line42 = file[42];
		const auto* line43 = file[43];

		ASSERT_NE(nullptr, line42);
		ASSERT_FALSE(line42->HasBeenExecuted());

		ASSERT_NE(nullptr, line43);		
		ASSERT_TRUE(line43->HasBeenExecuted());
	}
	
}