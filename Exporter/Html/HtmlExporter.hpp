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

#include <filesystem>
#include "../ExporterExport.hpp"

#include "TemplateHtmlExporter.hpp"
#include "HtmlFileCoverageExporter.hpp"
#include "../IExporter.hpp"

namespace Plugin
{
	class CoverageData;
	class FileCoverage;
	class ModuleCoverage;
}

namespace CppCoverage
{
	class CoverageRateComputer;
}

namespace boost
{
	template <typename T>
	class optional;
}

namespace Exporter
{
	class HtmlFolderStructure;

	class EXPORTER_DLL HtmlExporter: public IExporter
	{
	public:
		static const std::wstring WarningExitCodeMessage;

	public:
		explicit HtmlExporter(const std::filesystem::path& templateFolder, bool exportEmptyModules);

		std::filesystem::path GetDefaultPath(const std::wstring& prefix) const override;
		void Export(const Plugin::CoverageData&, const std::filesystem::path& outputFolder) override;

	private:
		HtmlExporter(const HtmlExporter&) = delete;
		HtmlExporter& operator=(const HtmlExporter&) = delete;

		boost::optional<std::filesystem::path> ExportFile(
			const HtmlFolderStructure& htmlFolderStructure,
			const Plugin::FileCoverage& fileCoverage) const;

		void ExportFiles(
			CppCoverage::CoverageRateComputer&,
			const Plugin::ModuleCoverage& module,
			const HtmlFolderStructure& htmlFolderStructure,
			ctemplate::TemplateDictionary& moduleTemplateDictionary);

	private:
		TemplateHtmlExporter exporter_;
		HtmlFileCoverageExporter fileCoverageExporter_;
		std::filesystem::path templateFolder_;
		bool exportEmptyModules_;
	};
}


