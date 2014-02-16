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
	TEST(ExecutedAddressManagerTest, RegisterAddress)
	{		
		cov::ExecutedAddressManager manager;

		ASSERT_THROW(manager.RegisterAddress(0, L"", 0, 0), cov::CppCoverageException);

		manager.SetCurrentModule(L"");
		manager.RegisterAddress(0, L"", 0, 0);

		ASSERT_THROW(manager.RegisterAddress(0, L"", 0, 0), cov::CppCoverageException);
	}

	TEST(ExecutedAddressManagerTest, MarkAddressAsExecuted)
	{
		cov::ExecutedAddressManager manager;

		manager.SetCurrentModule(L"");

		ASSERT_THROW(manager.MarkAddressAsExecuted(0), cov::CppCoverageException);

		manager.RegisterAddress(0, L"", 0, 0);
		ASSERT_NO_THROW(manager.MarkAddressAsExecuted(0));
	}	

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

		const cov::CoverageData coverageData = manager.CreateCoverageData(L"");
		
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