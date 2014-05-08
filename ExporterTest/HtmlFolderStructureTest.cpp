#include "stdafx.h"

#include "Exporter/Html/HtmlFolderStructure.hpp"

#include "Tools/TemporaryFolder.hpp"

namespace fs = boost::filesystem;

namespace ExporterTest
{
	namespace
	{
		//-------------------------------------------------------------------------
		struct HtmlFolderStructureTest : public ::testing::Test
		{
			HtmlFolderStructureTest()
			{
				boost::system::error_code error;
				auto folder = templateFolder_.GetPath() / Exporter::HtmlFolderStructure::CodePrettifyFolderName;
				fs::create_directories(folder);
				htmlFolderStructure_.reset(new Exporter::HtmlFolderStructure(templateFolder_));
			}

			std::unique_ptr<Exporter::HtmlFolderStructure> htmlFolderStructure_;

			Tools::TemporaryFolder templateFolder_;
			Tools::TemporaryFolder outputFolder_;
		};

		//-------------------------------------------------------------------------
		void CreateEmptyFile(const fs::path& path)
		{
			std::ofstream ofs;
			ofs.open(path.string(), std::ios::out);
		}

		//-------------------------------------------------------------------------
		const std::string Module = "Module";
		const std::string File = "file";
	}

	//-------------------------------------------------------------------------
	TEST_F(HtmlFolderStructureTest, CreateCurrentRoot)
	{								
		htmlFolderStructure_->CreateCurrentRoot(outputFolder_);
		auto createdPath = outputFolder_.GetPath() / Exporter::HtmlFolderStructure::CodePrettifyFolderName;
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
	TEST_F(HtmlFolderStructureTest, TestConflictRoot)
	{		
		auto output = outputFolder_.GetPath() / "Output";

		auto root = htmlFolderStructure_->CreateCurrentRoot(output);
		auto otherRoot = htmlFolderStructure_->CreateCurrentRoot(output);

		ASSERT_NE(root, otherRoot);
	}

	//-------------------------------------------------------------------------
	TEST_F(HtmlFolderStructureTest, TestConflictModules)
	{
		htmlFolderStructure_->CreateCurrentRoot(outputFolder_);		

		auto module = htmlFolderStructure_->CreateCurrentModule(Module);
		auto modulePath = module.GetAbsolutePath();

		CreateEmptyFile(modulePath);
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

		CreateEmptyFile(filePath);
		auto otherFile = htmlFolderStructure_->GetHtmlFilePath(File);

		ASSERT_NE(filePath, otherFile.GetAbsolutePath());
	}
}