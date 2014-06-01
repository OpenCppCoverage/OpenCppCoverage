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

#include "../ExporterExport.hpp"

#include "TemplateHtmlExporter.hpp"
#include "HtmlFileCoverageExporter.hpp"

namespace CppCoverage
{
	class CoverageData;
	class FileCoverage;
}

namespace boost
{
	template <typename T>
	class optional;

	namespace filesystem
	{
		class path;
	}
}

namespace Exporter
{
	class HtmlFolderStructure;

	class EXPORTER_DLL HtmlExporter
	{
	public:
		static const std::wstring WarningExitCodeMessage;

	public:
		explicit HtmlExporter(const boost::filesystem::path& templateFolder);

		void Export(const CppCoverage::CoverageData&, const boost::filesystem::path& outputFolder) const;

	private:
		HtmlExporter(const HtmlExporter&) = delete;
		HtmlExporter& operator=(const HtmlExporter&) = delete;

		boost::optional<boost::filesystem::path> ExportFile(
			const HtmlFolderStructure& htmlFolderStructure,
			const CppCoverage::FileCoverage& fileCoverage) const;

		void ExportFiles(
			const CppCoverage::ModuleCoverage& module,
			const HtmlFolderStructure& htmlFolderStructure,
			ctemplate::TemplateDictionary& moduleTemplateDictionary) const;

	private:
		TemplateHtmlExporter exporter_;
		HtmlFileCoverageExporter fileCoverageExporter_;
		boost::filesystem::path templateFolder_;
	};
}


