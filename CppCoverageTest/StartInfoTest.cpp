#include "stdafx.h"

#include <boost/filesystem.hpp>

#include "CppCoverage/StartInfo.hpp"
#include "Tools/ExceptionBase.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

namespace cov = CppCoverage;
namespace fs = boost::filesystem;

namespace CppCoverageTest
{
	namespace
	{
		const std::wstring validFilename = TestCoverageConsole::GetOutputBinaryPath().wstring(); 
	}

	//-------------------------------------------------------------------------
	TEST(StartInfoTest, Constructor)
	{
		ASSERT_NO_THROW(cov::StartInfo s{ validFilename };);
	}

	//-------------------------------------------------------------------------
	TEST(StartInfoTest, ConstructorFileNotFound)
	{		
		ASSERT_THROW(cov::StartInfo s(L""), Tools::ExceptionBase);
	}

	//-------------------------------------------------------------------------
	TEST(StartInfoTest, SetWorkingDirectoryNotExists)
	{
		cov::StartInfo s(validFilename);
		fs::path folder{ L"" };
		
		ASSERT_THROW(s.SetWorkingDirectory(folder), Tools::ExceptionBase);
	}

	//-------------------------------------------------------------------------
	TEST(StartInfoTest, SetWorkingDirectoryExists)
	{
		cov::StartInfo s(validFilename);
		fs::path folder{ L"." };

		ASSERT_NO_THROW(s.SetWorkingDirectory(folder));
	}

}