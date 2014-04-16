#include "stdafx.h"
#include "HtmlFolderStructure.hpp"

#include "Exporter\ExporterException.hpp"

#include "Tools/Tool.hpp"

namespace fs = boost::filesystem;

namespace Exporter
{
	namespace
	{		
		//---------------------------------------------------------------------
		void CopyRecursiveDirectoryContent(
			const fs::path& from,
			const fs::path& to)
		{
			fs::create_directory(to);
			for (fs::recursive_directory_iterator it(from);
				it != fs::recursive_directory_iterator(); ++it)
			{
				const auto& path = it->path();

				fs::copy_file(path, to / path.filename(), fs::copy_option::overwrite_if_exists);
			}
		}

		//---------------------------------------------------------------------
		fs::path CreateUniqueDirectories(const fs::path& initialPath)
		{
			auto uniquePath = Tools::Tool::GetUniquePath(initialPath);
			
			fs::create_directories(uniquePath);
			
			return uniquePath;
		}
	}

	//-------------------------------------------------------------------------
	const std::wstring HtmlFolderStructure::CodePrettifyFolderName = L"google-code-prettify";
	const std::wstring HtmlFolderStructure::FolderModules = L"Modules";

	//-------------------------------------------------------------------------
	HtmlFolderStructure::HtmlFolderStructure(const boost::filesystem::path& templateFolder)
		: templateFolder_(templateFolder)
	{
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path HtmlFolderStructure::CreateCurrentRoot(const boost::filesystem::path& outputFolder)
	{
		currentRoot_ = CreateUniqueDirectories(fs::absolute(outputFolder));
		CopyRecursiveDirectoryContent(
			templateFolder_ / HtmlFolderStructure::CodePrettifyFolderName,
			*currentRoot_ / HtmlFolderStructure::CodePrettifyFolderName);

		return *currentRoot_;
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path HtmlFolderStructure::CreateCurrentModule(const boost::filesystem::path& modulePath)
	{
		if (!currentRoot_)
			THROW(L"No root is selected");

		auto moduleFilename = modulePath.filename();
		auto moduleName = moduleFilename.replace_extension(""); // remove extension
		auto modulesPath = *currentRoot_ / HtmlFolderStructure::FolderModules;
		currentModule_ = CreateUniqueDirectories(modulesPath / moduleName);
		
		auto moduleHtmlName = moduleName.string() + ".html";
		auto moduleHtmlPath = modulesPath / moduleHtmlName;
		
		return Tools::Tool::GetUniquePath(moduleHtmlPath);
	}	

	//---------------------------------------------------------------------
	fs::path HtmlFolderStructure::GetHtmlFilePath(const boost::filesystem::path& filePath) const
	{
		if (!currentModule_)
			THROW(L"No root module selected");
		
		auto filename = filePath.filename();
		auto output = filename.replace_extension("html");
		auto fileHtmlPath = *currentModule_ / output;

		return Tools::Tool::GetUniquePath(fileHtmlPath);
	}

	//---------------------------------------------------------------------
	boost::filesystem::path HtmlFolderStructure::GetCodeCssPath() const
	{
		if (!currentRoot_)
			THROW(L"No root is selected");

		return *currentRoot_ / CodePrettifyFolderName / "prettify-CppCoverage.css";		
	}

	//---------------------------------------------------------------------
	boost::filesystem::path HtmlFolderStructure::GetCodePrettifyPath() const
	{
		if (!currentRoot_)
			THROW(L"No root is selected");

		return *currentRoot_ / CodePrettifyFolderName / "prettify.js";
	}
}
