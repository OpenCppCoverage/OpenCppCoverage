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
#include "Plugin/Exporter/CoverageData.hpp"
#include "Plugin/Exporter/ModuleCoverage.hpp"
#include "Plugin/Exporter/FileCoverage.hpp"
#include "Plugin/Exporter/LineCoverage.hpp"
#include "CppCoverage/Address.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		#pragma warning(push)
		#pragma warning(disable: 4312) // 'reinterpret_cast': conversion from 'int' to 'void *' of greater size
		//-------------------------------------------------------------------------
		cov::Address CreateAddress(int addressValue)
		{
			return cov::Address{ nullptr, reinterpret_cast<void*>(addressValue) };
		}

		#pragma warning(pop)
	}

	//-------------------------------------------------------------------------
	TEST(ExecutedAddressManagerTest, RegisterAddress)
	{		
		cov::ExecutedAddressManager manager;
		cov::Address address = CreateAddress(0);

		ASSERT_THROW(manager.RegisterAddress(address, L"", 0, 0), cov::CppCoverageException);

		manager.AddModule(L"", nullptr);
		manager.RegisterAddress(address, L"", 0, 0);
	}

	//-------------------------------------------------------------------------
	TEST(ExecutedAddressManagerTest, MarkAddressAsExecuted)
	{
		cov::ExecutedAddressManager manager;
		cov::Address address = CreateAddress(0);

		manager.AddModule(L"", nullptr);

		ASSERT_EQ(boost::none, manager.MarkAddressAsExecuted(address));

		manager.RegisterAddress(address, L"", 0, 0);
		ASSERT_NO_THROW(manager.MarkAddressAsExecuted(address));
	}	

	//-------------------------------------------------------------------------
	TEST(ExecutedAddressManagerTest, CreateCoverageData)
	{
		cov::ExecutedAddressManager manager;
		
		const std::wstring moduleName = L"module";
		const std::wstring filename = L"filename";
		const char instructionLine42 = 10;
		const char instructionLine43 = 11;
		cov::Address address1 = CreateAddress(1);
		cov::Address address2 = CreateAddress(2); 
		HANDLE hProcess = nullptr;
		manager.AddModule(moduleName, nullptr);
		manager.RegisterAddress(address1, filename, 42, instructionLine42);
		manager.RegisterAddress(address2, filename, 43, instructionLine43);
		manager.MarkAddressAsExecuted(address2);
		manager.OnExitProcess(hProcess);

		const Plugin::CoverageData coverageData = manager.CreateCoverageData(L"", 0);
		
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

	//-------------------------------------------------------------------------
	TEST(ExecutedAddressManagerTest, AddSameModuleTwice)
	{
		cov::ExecutedAddressManager manager;
		const auto moduleName1 = L"moduleName1";
		const auto moduleName2 = L"moduleName2";

		manager.AddModule(moduleName1, nullptr);
		manager.AddModule(moduleName2, nullptr);
		manager.AddModule(moduleName1, nullptr);

		auto coverageData = manager.CreateCoverageData(L"", 0);

		const auto& modules = coverageData.GetModules();
		ASSERT_EQ(2, modules.size());
		ASSERT_EQ(moduleName1, modules.at(0)->GetPath().wstring());
		ASSERT_EQ(moduleName2, modules.at(1)->GetPath().wstring());
	}
}