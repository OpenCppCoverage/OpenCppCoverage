// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2019 OpenCppCoverage
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
#include <memory>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>

#include "CppCoverage/ExportPluginDescription.hpp"

namespace CppCoverage
{
	class CoverageData;
}

namespace Exporter
{
	template <typename T>
	class IPluginLoader;

	template <typename T>
	class LoadedPlugin;

	class IExportPlugin;

	class EXPORTER_DLL ExporterPluginManager
	{
	  public:
		explicit ExporterPluginManager(IPluginLoader<IExportPlugin>&,
		                               std::filesystem::path&& pluginFolder);
		~ExporterPluginManager();

		ExporterPluginManager(const ExporterPluginManager&) = delete;
		ExporterPluginManager(ExporterPluginManager&&) = delete;

		ExporterPluginManager& operator=(const ExporterPluginManager&) = delete;
		ExporterPluginManager& operator=(ExporterPluginManager&&) = delete;

		std::vector<CppCoverage::ExportPluginDescription>
		CreateExportPluginDescriptions() const;

		void Export(const std::wstring& pluginName,
		            const CppCoverage::CoverageData&,
		            const std::wstring& argument) const;

	  private:
		std::unordered_map<std::wstring,
		                   std::shared_ptr<LoadedPlugin<IExportPlugin>>>
		    plugins_;
		std::filesystem::path pluginFolder_;
	};
}
