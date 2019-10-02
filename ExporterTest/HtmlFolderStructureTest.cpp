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

#include <system_error>

#include "Exporter/Html/HtmlFolderStructure.hpp"

#include "TestHelper/TemporaryPath.hpp"
#include "TestHelper/Tools.hpp"

namespace fs = std::filesystem;

namespace ExporterTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		struct HtmlFolderStructureTest : public ::testing::Test
		{
			HtmlFolderStructureTest()
			{
				std::error_code error;
				auto folder = templateFolder_.GetPath() / Exporter::HtmlFolderStructure::ThirdParty;
				fs::create_directories(folder);				
				htmlFolderStructure_.reset(new Exporter::HtmlFolderStructure(templateFolder_));				
			}

			std::unique_ptr<Exporter::HtmlFolderStructure> htmlFolderStructure_;

			TestHelper::TemporaryPath templateFolder_;
			TestHelper::TemporaryPath outputFolder_;
		};
		
		//-------------------------------------------------------------------------
		const std::string Module = "Module";
		const std::string File = "file";
	}

	//-------------------------------------------------------------------------
	TEST_F(HtmlFolderStructureTest, CreateCurrentRoot)
	{						
		TestHelper::CreateEmptyFile(templateFolder_.GetPath() / Exporter::HtmlFolderStructure::ThirdParty / "EmptyFile");
		htmlFolderStructure_->CreateCurrentRoot(outputFolder_);
		auto createdPath = outputFolder_.GetPath() / Exporter::HtmlFolderStructure::ThirdParty;
		ASSERT_TRUE(fs::exists(createdPath));	
	}
	
	//-------------------------------------------------------------------------
	TEST_F(HtmlFolderStructureTest, CreateCurrentModule)
	{		
		htmlFolderStructure_->CreateCurrentRoot(outputFolder_);
		auto htmlPath = htmlFolderStructure_->CreateCurrentModule(Module + ".exe");
		auto modulesFolder = outputFolder_.GetPath() / Exporter::HtmlFolderStructure::FolderModules;
		auto expectedOutputFolder = modulesFolder / Module;
		ASSERT_TRUE(fs::exists(expectedOutputFolder));
		auto expectedPath = modulesFolder / (Module + ".html");
		ASSERT_EQ(expectedPath, htmlPath.GetAbsolutePath());
	}

	//-------------------------------------------------------------------------
	TEST_F(HtmlFolderStructureTest, GetHtmlFilePath)
	{		
		htmlFolderStructure_->CreateCurrentRoot(outputFolder_);
		htmlFolderStructure_->CreateCurrentModule(Module);
		auto htmlFilePath = htmlFolderStructure_->GetHtmlFilePath(File);

		auto expectedPath = outputFolder_.GetPath();
		
		expectedPath /= Exporter::HtmlFolderStructure::FolderModules;		
		expectedPath /= Module;
		expectedPath /= File + ".html";
		ASSERT_EQ(expectedPath, htmlFilePath.GetAbsolutePath());
	}

	//-------------------------------------------------------------------------
	TEST_F(HtmlFolderStructureTest, TestConflictModules)
	{
		htmlFolderStructure_->CreateCurrentRoot(outputFolder_);		

		auto module = htmlFolderStructure_->CreateCurrentModule(Module);
		auto modulePath = module.GetAbsolutePath();

		TestHelper::CreateEmptyFile(modulePath);
		auto otherModule = htmlFolderStructure_->CreateCurrentModule(Module);

		ASSERT_NE(modulePath, otherModule.GetAbsolutePath());
	} 

	//-------------------------------------------------------------------------
	TEST_F(HtmlFolderStructureTest, TestConflictFile)
	{
		htmlFolderStructure_->CreateCurrentRoot(outputFolder_);
		htmlFolderStructure_->CreateCurrentModule(Module);

		auto file = htmlFolderStructure_->GetHtmlFilePath(File);
		auto filePath = file.GetAbsolutePath();

		TestHelper::CreateEmptyFile(filePath);
		auto otherFile = htmlFolderStructure_->GetHtmlFilePath(File);

		ASSERT_NE(filePath, otherFile.GetAbsolutePath());
	}
}