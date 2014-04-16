#pragma once

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "../ExporterExport.hpp"

namespace Exporter
{	
	class EXPORTER_DLL HtmlFolderStructure
	{
	public:
		static const std::wstring CodePrettifyFolderName;
		static const std::wstring FolderModules;

	public:
		HtmlFolderStructure(const boost::filesystem::path& templateFolder);

		boost::filesystem::path CreateCurrentRoot(const boost::filesystem::path& outputFolder);
		boost::filesystem::path CreateCurrentModule(const boost::filesystem::path&);

		boost::filesystem::path GetHtmlFilePath(const boost::filesystem::path& filePath) const;
		boost::filesystem::path GetCodeCssPath() const;
		boost::filesystem::path GetCodePrettifyPath() const;

	private:
		HtmlFolderStructure(const HtmlFolderStructure&) = delete;
		HtmlFolderStructure& operator=(const HtmlFolderStructure&) = delete;

	private:
		boost::filesystem::path templateFolder_;
		boost::optional<boost::filesystem::path> currentRoot_;
		boost::optional<boost::filesystem::path> currentModule_;
	};
}

