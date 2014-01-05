#include "stdafx.h"

#include "CppCoverage/ExecutedAddressManager.hpp"
#include "CppCoverage/CppCoverageException.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	TEST(ExecutedAddressManagerTest, AddAddress)
	{		
		cov::ExecutedAddressManager manager;

		ASSERT_THROW(manager.AddAddress(0, L"", 0, 0), cov::CppCoverageException);

		manager.SetCurrentModule(L"");
		manager.AddAddress(0, L"", 0, 0);

		ASSERT_THROW(manager.AddAddress(0, L"", 0, 0), cov::CppCoverageException);
	}

	TEST(ExecutedAddressManagerTest, MarkAddressAsExecuted)
	{
		cov::ExecutedAddressManager manager;

		manager.SetCurrentModule(L"");

		ASSERT_THROW(manager.MarkAddressAsExecuted(0), cov::CppCoverageException);

		manager.AddAddress(0, L"", 0, 0);
		ASSERT_NO_THROW(manager.MarkAddressAsExecuted(0));
	}	

	TEST(ExecutedAddressManagerTest, CreateCoverageData)
	{
		/*cov::ExecutedAddressManager manager;
		const std::wstring moduleName = L"module";

		
		ASSERT_THROW(manager.MarkAddressAsExecuted(0), cov::CppCoverageException);

		manager.AddAddress(0, L"", 0, 0);
		ASSERT_NO_THROW(manager.MarkAddressAsExecuted(0));*/
	}
	
}