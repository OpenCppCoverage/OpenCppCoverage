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

#include <memory>

#include "CppCoverage/CoverageRateComputer.hpp"
#include "Plugin/Exporter/CoverageData.hpp"
#include "Plugin/Exporter/ModuleCoverage.hpp"
#include "Plugin/Exporter/FileCoverage.hpp"
#include "CppCoverage/CoverageRate.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		//---------------------------------------------------------------------
		struct CoverageRateComputerTest : public testing::Test
		{
			//-----------------------------------------------------------------
			CoverageRateComputerTest()
				: coverageData_(L"", 0)				
			{				
				module1_ = &coverageData_.AddModule(L"m1");
				module2_ = &coverageData_.AddModule(L"m2");

				file1_ = &module1_->AddFile(L"f1");
				file2_ = &module1_->AddFile(L"f2");
				file3_ = &module2_->AddFile(L"f3");

				SetFileLineCount(*file1_, 3, 4);
				SetFileLineCount(*file2_, 1, 2);
				SetFileLineCount(*file3_, 5, 10);

				coverageRateComputer_.reset(new cov::CoverageRateComputer{ coverageData_ });
			}

			//-----------------------------------------------------------------
			static void SetFileLineCount(
				Plugin::FileCoverage& file, 
				int executedCount, 
				int unexecutedCount)
			{
				int line = 0;

				for (int i = 0; i < executedCount; ++i)
					file.AddLine(++line, true);
				for (int i = 0; i < unexecutedCount; ++i)
					file.AddLine(++line, false);
			}

			Plugin::CoverageData coverageData_;
			std::unique_ptr<cov::CoverageRateComputer> coverageRateComputer_;
			Plugin::ModuleCoverage* module1_;
			Plugin::ModuleCoverage* module2_;
			Plugin::FileCoverage* file1_;
			Plugin::FileCoverage* file2_;
			Plugin::FileCoverage* file3_;
		};
	}

	//-------------------------------------------------------------------------
	TEST_F(CoverageRateComputerTest, FileCoverage)
	{
		const auto& rate = coverageRateComputer_->GetCoverageRate(*file2_);

		ASSERT_EQ(1, rate.GetExecutedLinesCount());
		ASSERT_EQ(2, rate.GetUnExecutedLinesCount());
	}

	//-------------------------------------------------------------------------
	TEST_F(CoverageRateComputerTest, ModuleCoverage)
	{
		const auto& rate = coverageRateComputer_->GetCoverageRate(*module1_);

		ASSERT_EQ(1 + 3, rate.GetExecutedLinesCount());
		ASSERT_EQ(2 + 4, rate.GetUnExecutedLinesCount());
	}

	//-------------------------------------------------------------------------
	TEST_F(CoverageRateComputerTest, CoverageRate)
	{
		const auto& rate = coverageRateComputer_->GetCoverageRate();

		ASSERT_EQ(1 + 3 + 5, rate.GetExecutedLinesCount());
		ASSERT_EQ(2 + 4 + 10, rate.GetUnExecutedLinesCount());
	}

	//-------------------------------------------------------------------------
	TEST_F(CoverageRateComputerTest, SortModulesByCoverageRate)
	{
		const auto& modules = coverageRateComputer_->SortModulesByCoverageRate();

		ASSERT_EQ(2, modules.size());
		ASSERT_EQ(module2_->GetPath(), modules[0]->GetPath());
		ASSERT_EQ(module1_->GetPath(), modules[1]->GetPath());		
	}

	//-------------------------------------------------------------------------
	TEST_F(CoverageRateComputerTest, SortFileByCoverageRate)
	{
		const auto& files = coverageRateComputer_->SortFilesByCoverageRate(*module1_);

		ASSERT_EQ(2, files.size());
		ASSERT_EQ(file2_->GetPath(), files[0]->GetPath());
		ASSERT_EQ(file1_->GetPath(), files[1]->GetPath());
	}
}