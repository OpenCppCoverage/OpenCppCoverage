// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2018 OpenCppCoverage
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

#include "CppCoverage/FilterAssistant.hpp"
#include <boost/optional/optional.hpp>
#include <filesystem>
#include "FileSystemMock.hpp"

namespace cov = CppCoverage;
using namespace testing;

namespace
{
	//-------------------------------------------------------------------------
	struct Params
	{
		void (cov::FilterAssistant::*onNewFile)(const std::filesystem::path&,
		                                        bool);
		boost::optional<std::filesystem::path> (
		    cov::FilterAssistant::*computeSuggestedFilter)() const;
	};
}

namespace CppCoverageTest
{
	//-------------------------------------------------------------------------
	struct FilterAssistantTest : public TestWithParam<Params>
	{
		//---------------------------------------------------------------------
		FilterAssistantTest() : fileSystem_{std::make_shared<FileSystemMock>()}
		{
			filterAssistant_ = std::make_unique<cov::FilterAssistant>(
			    std::shared_ptr<cov::IFileSystem>{fileSystem_});
		}

		//---------------------------------------------------------------------
		void AddFile(const std::filesystem::path& path,
					 int time,
		             bool isSelected)
		{
			EXPECT_CALL(*fileSystem_, GetLastWriteTime(path))
			    .WillRepeatedly(Return(std::filesystem::file_time_type(std::chrono::seconds(time))));
			(*filterAssistant_.*GetParam().onNewFile)(path, isSelected);
		}

		//---------------------------------------------------------------------
		boost::optional<std::filesystem::path> ComputeSuggestedFilter() const
		{
			return (*filterAssistant_.*GetParam().computeSuggestedFilter)();
		}

	  private:
		std::shared_ptr<FileSystemMock> fileSystem_;
		std::unique_ptr<cov::FilterAssistant> filterAssistant_;
	};

	//-------------------------------------------------------------------------
	TEST_P(FilterAssistantTest, ComputeSuggestSourceFile)
	{
		ASSERT_EQ(boost::none, ComputeSuggestedFilter());

		AddFile("folder1/file1", 1, false);
		AddFile("folder3/file3", 3, false);
		AddFile("folder2/file2", 2, false);

		auto suggestedFile = ComputeSuggestedFilter();
		ASSERT_TRUE(suggestedFile);
		ASSERT_EQ(L"folder3", suggestedFile->wstring());

		AddFile("file4", 2, true);
		AddFile("file5", 2, false);
		ASSERT_EQ(boost::none, ComputeSuggestedFilter());
	}

	//-------------------------------------------------------------------------
	INSTANTIATE_TEST_SUITE_P(
	    FilterAssistantTest,
	    FilterAssistantTest,
	    ::testing::Values(
	        Params{&cov::FilterAssistant::OnNewModule,
	               &cov::FilterAssistant::ComputeSuggestedModuleFilter},
	        Params{&cov::FilterAssistant::OnNewSourceFile,
	               &cov::FilterAssistant::ComputeSuggestedSourceFileFilter}));
}
