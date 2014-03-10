#include "stdafx.h"

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/CppCoverageException.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		const std::string file = __FILE__;
		const std::wstring validFilename(file.begin(), file.end());
	}

	TEST(StartInfoTest, Constructor)
	{
		ASSERT_NO_THROW(cov::StartInfo s{ validFilename };);
	}

	TEST(StartInfoTest, ConstructorFileNotFound)
	{
		
	/*	ASSERT_THROW(cov::StartInfo s(L""), cov::CppCoverageException);*/ //$$
	}

	TEST(StartInfoTest, SetWorkingDirectoryNotExists)
	{
		/*cov::StartInfo s(validFilename);
		std::wstring folder{ L"" };
		// $$
		ASSERT_THROW(s.SetWorkingDirectory(&folder), cov::CppCoverageException);*/
	}

	TEST(StartInfoTest, SetWorkingDirectoryExists)
	{
		cov::StartInfo s(validFilename);
		std::wstring folder{ L"." };

		ASSERT_NO_THROW(s.SetWorkingDirectory(&folder));
	}

}