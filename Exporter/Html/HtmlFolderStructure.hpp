#pragma once

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "HtmlFile.hpp"
#include "../ExporterExport.hpp"

namespace Exporter
{	
	class EXPORTER_DLL HtmlFolderStructure
	{
	public:
		static const std::wstring ThirdParty;
		static const std::wstring FolderModules;

	public:
		HtmlFolderStructure(const boost::filesystem::path& templateFolder);

		boost::filesystem::path CreateCurrentRoot(const boost::filesystem::path& outputFolder);
		HtmlFile CreateCurrentModule(const boost::filesystem::path&);		
		HtmlFile GetHtmlFilePath(const boost::filesystem::path& filePath) const;

	private:
		HtmlFolderStructure(const HtmlFolderStructure&) = delete;
		HtmlFolderStructure& operator=(const HtmlFolderStructure&) = delete;

	private:
		boost::filesystem::path templateFolder_;
		boost::optional<boost::filesystem::path> currentRoot_;
		boost::optional<boost::filesystem::path> currentModule_;
	};
}

