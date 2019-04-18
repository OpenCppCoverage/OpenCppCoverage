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

#include <random>
#include <filesystem>

#include "CppCoverage/CoverageDataMerger.hpp"
#include "CppCoverage/CoverageData.hpp" 
#include "CppCoverage/ModuleCoverage.hpp" 
#include "CppCoverage/FileCoverage.hpp" 
#include "CppCoverage/LineCoverage.hpp" 

namespace cov = CppCoverage;
namespace fs = std::filesystem;

namespace CppCoverageTest
{	
	namespace
	{
		//---------------------------------------------------------------------
		const fs::path modulePath = L"modulePath";
		const fs::path filePath = L"filePath";

		//---------------------------------------------------------------------
		std::vector<cov::CoverageData> CreateCoverageDataCollection(
			const std::initializer_list<std::pair<std::wstring, int>>& coverageDataArguments)
		{
			std::vector<cov::CoverageData> coverageDataCollection;

			for (const auto& args : coverageDataArguments)
				coverageDataCollection.emplace_back(cov::CoverageData{ args.first, args.second });
			
			return coverageDataCollection;
		}

		//---------------------------------------------------------------------
		std::vector<cov::CoverageData> CreateCoverageDataCollection(size_t count)
		{
			std::vector<cov::CoverageData> coverageDataCollection;

			for (size_t i = 0; i < count; ++i)
				coverageDataCollection.emplace_back(cov::CoverageData{ L"", 0 });

			return coverageDataCollection;
		}

		//-------------------------------------------------------------------------
		void AddLinesToFileCoverage(
			cov::FileCoverage& fileCoverage,
			const std::initializer_list<std::pair<int, bool>>& lineArgCollection)
		{
			for (const auto& lineArg : lineArgCollection)
				fileCoverage.AddLine(lineArg.first, lineArg.second);
		}

		//-------------------------------------------------------------------------
		void AddLine(
			cov::CoverageData& coverageData,
			const fs::path& modulePath,
			const fs::path& filePath,
			const std::initializer_list<std::pair<int, bool>>& lineArgCollection)
		{
			auto& file = coverageData.AddModule(modulePath).AddFile(filePath);

			AddLinesToFileCoverage(file, lineArgCollection);
		}

		//-------------------------------------------------------------------------
		void CheckLineHasBeenExecuted(
			const std::unique_ptr<cov::FileCoverage>& file,
			int lineNumber,
			bool exectedValue)
		{
			ASSERT_NE(nullptr, file);
			auto* line = (*file)[lineNumber];

			ASSERT_NE(nullptr, line);
			ASSERT_EQ(exectedValue, line->HasBeenExecuted());
		}
	}

	//-------------------------------------------------------------------------
	TEST(CoverageDataMergerTest, CoverageData)
	{		 
		auto coverageDatas = CreateCoverageDataCollection({ { L"1", 0 }, { L"1", 0 } });
		auto coverageDataMerged = cov::CoverageDataMerger{}.Merge(coverageDatas);

		ASSERT_NE(std::wstring{}, coverageDataMerged.GetName());
		ASSERT_EQ(0, coverageDataMerged.GetExitCode());
	}

	//-------------------------------------------------------------------------
	TEST(CoverageDataMergerTest, NotZeroExitCode)
	{
		auto exitCode = 42;
		auto coverageDatas = CreateCoverageDataCollection({ { L"", 0 }, { L"", exitCode } });
		auto coverageDataMerged = cov::CoverageDataMerger{}.Merge(coverageDatas);

		ASSERT_EQ(exitCode, coverageDataMerged.GetExitCode());
	}
	
	//-------------------------------------------------------------------------
	TEST(CoverageDataMergerTest, Module)
	{
		auto coverageDatas = CreateCoverageDataCollection(2);

		AddLine(coverageDatas[0], "m1", "f1", {});
		AddLine(coverageDatas[1], "m2", "f2", {});

		auto coverageDataMerged = cov::CoverageDataMerger{}.Merge(coverageDatas);
		ASSERT_EQ(2, coverageDataMerged.GetModules().size());
	}

	//-------------------------------------------------------------------------
	TEST(CoverageDataMergerTest, File)
	{
		auto coverageDatas = CreateCoverageDataCollection(2);

		AddLine(coverageDatas[0], modulePath, "f1", {});
		AddLine(coverageDatas[1], modulePath, "f2", {});

		auto coverageDataMerged = cov::CoverageDataMerger{}.Merge(coverageDatas);
		const auto& mergedModule = coverageDataMerged.GetModules().at(0);
		ASSERT_NE(nullptr, mergedModule);
		ASSERT_EQ(2, mergedModule->GetFiles().size());
	}

	//-------------------------------------------------------------------------
	TEST(CoverageDataMergerTest, Line)
	{		
		auto coverageDatas = CreateCoverageDataCollection(2);				
				
		AddLine(coverageDatas[0], modulePath, filePath, { { 0, false }, { 1, false }, { 2, true } });
		AddLine(coverageDatas[1], modulePath, filePath, { { 1, true }, { 2, false }, { 3, true } });
		
		auto coverageDataMerged = cov::CoverageDataMerger{}.Merge(coverageDatas);
		const auto& mergedModule = coverageDataMerged.GetModules().at(0);
		ASSERT_NE(nullptr, mergedModule);
		const auto& mergedFile = mergedModule->GetFiles().at(0);
		ASSERT_NE(nullptr, mergedModule);
		ASSERT_EQ(4, mergedFile->GetLines().size());

		CheckLineHasBeenExecuted(mergedFile, 0, false);
		CheckLineHasBeenExecuted(mergedFile, 1, true);
		CheckLineHasBeenExecuted(mergedFile, 2, true);
		CheckLineHasBeenExecuted(mergedFile, 3, true);
	}

	//-------------------------------------------------------------------------
	TEST(CoverageDataMergerTest, MergeFileCoverageEmpty)
	{
		cov::CoverageData coverageData{L"test", 0};

		AddLine(coverageData, modulePath, filePath, { { 0, false }, { 1, false }, { 2, true } });
		cov::CoverageDataMerger{}.MergeFileCoverage(coverageData);

		auto& mergedFile = coverageData.GetModules().at(0)->GetFiles().at(0);
		ASSERT_EQ(3, mergedFile->GetLines().size());
		CheckLineHasBeenExecuted(mergedFile, 0, false);
		CheckLineHasBeenExecuted(mergedFile, 1, false);
		CheckLineHasBeenExecuted(mergedFile, 2, true);
	}

	//-------------------------------------------------------------------------
	TEST(CoverageDataMergerTest, MergeFileCoverageMultipleFiles)
	{
		cov::CoverageData coverageData{ L"test", 0 };

		auto& fileCoverage1 = coverageData.AddModule(modulePath).AddFile(filePath);
		auto& fileCoverage2 = coverageData.AddModule(L"otherModule").AddFile(filePath);

		AddLinesToFileCoverage(fileCoverage1, { { 0, false }, { 1, false }, { 2, true } });
		AddLinesToFileCoverage(fileCoverage2, { { 1, true }, { 2, false }, { 3, true } });
		cov::CoverageDataMerger{}.MergeFileCoverage(coverageData);

		const auto& modules = coverageData.GetModules();
		ASSERT_EQ(2, modules.size());
		for (const auto& module : modules)
		{
			auto& mergedFile = module->GetFiles().at(0);
			ASSERT_EQ(4, mergedFile->GetLines().size());
			CheckLineHasBeenExecuted(mergedFile, 0, false);
			CheckLineHasBeenExecuted(mergedFile, 1, true);
			CheckLineHasBeenExecuted(mergedFile, 2, true);
			CheckLineHasBeenExecuted(mergedFile, 3, true);
		}
	}
}