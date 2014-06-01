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

