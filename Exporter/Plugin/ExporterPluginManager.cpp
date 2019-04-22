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

#include "stdafx.h"
#include "ExporterPluginManager.hpp"
#include "IPluginLoader.hpp"
#include "Plugin.hpp"
#include "IExportPlugin.hpp"
#include "../ExporterException.hpp"

namespace Exporter
{
	namespace
	{
		//---------------------------------------------------------------------
		std::string InvalidPluginError(const std::string& functionName,
		                               const std::string& additionalInformation,
		                               const std::filesystem::path& pluginPath)
		{
			std::string fullMessage =
			    "There was an error when trying to load the export plugin " +
			    pluginPath.string() + '\n';
			fullMessage += "Error: Function " + functionName + " failed: ";
			fullMessage += additionalInformation;
			return fullMessage;
		}

		//---------------------------------------------------------------------
		template <typename PluginFct, typename... Args>
		decltype(auto)
		CallPluginfunction(PluginFct pluginFct,
		                   const std::string& functionName,
		                   const std::filesystem::path& pluginPath,
		                   Args&&... args)
		{
			std::string error;

			try
			{
				return pluginFct(std::forward<Args>(args)...);
			}
			catch (const std::exception& e)
			{
				error = e.what();
			}
			catch (...)
			{
				error = "Unknown";
			}
			throw std::runtime_error(
			    InvalidPluginError(functionName, error, pluginPath));
		}
	}

	//-------------------------------------------------------------------------
	ExporterPluginManager::ExporterPluginManager(
	    IPluginLoader<IExportPlugin>& pluginLoader,
	    std::filesystem::path&& pluginFolder)
	    : pluginFolder_{std::move(pluginFolder)}
	{
		for (auto pluginPath :
		     std::filesystem::directory_iterator(pluginFolder))
		{
			auto path = pluginPath.path();

			if (path.extension() != ".dll")
				continue;

			auto pluginName = path.stem().wstring();
			std::shared_ptr<Plugin<IExportPlugin>> plugin;
			const std::string pluginFactoryFctName = "CreatePlugin";
			std::string error;
			try
			{
				plugin = pluginLoader.TryLoadPlugin(pluginPath,
				                                    pluginFactoryFctName);
			}
			catch (const InvalidPluginException&)
			{
				throw;
			}
			catch (const std::exception& e)
			{
				error = e.what();
			}
			catch (...)
			{
				error = "Unknown";
			}

			if (!error.empty())
			{
				throw std::runtime_error(InvalidPluginError(
				    pluginFactoryFctName, error, pluginPath));
			}

			plugins_.emplace(pluginName, std::move(plugin));
		}
	}

	//-------------------------------------------------------------------------
	ExporterPluginManager::~ExporterPluginManager() = default;

	//-------------------------------------------------------------------------
	std::vector<CppCoverage::ExportPluginDescription>
	ExporterPluginManager::CreateExportPluginDescriptions() const
	{
		std::vector<CppCoverage::ExportPluginDescription>
		    exportPluginDescriptions;

		for (const auto& plugin : plugins_)
		{
			const auto& pluginName = plugin.first;
			const auto& exportPlugin = plugin.second;
			const auto pluginPath = pluginFolder_ / (pluginName + L".dll");

			auto helpDescription = CallPluginfunction(
			    [&]() { return exportPlugin->Get().GetHelpDescription(); },
			    "GetHelpDescription",
			    pluginPath);

			std::weak_ptr<Plugin<IExportPlugin>> weakPlugin{exportPlugin};

			exportPluginDescriptions.push_back(
			    CppCoverage::ExportPluginDescription{
			        std::wstring{pluginName},
			        std::move(helpDescription),
			        [weakPlugin, pluginPath, this](
			            const std::wstring& parameter) {
				        auto p = weakPlugin.lock();
				        if (!p)
					        THROW("Plugin was released");

				        CallPluginfunction(
				            [&]() { p->Get().CheckArgument(parameter); },
				            "CheckArgument",
				            pluginPath);
			        }});
		}

		return exportPluginDescriptions;
	}

	//-------------------------------------------------------------------------
	void
	ExporterPluginManager::Export(const std::wstring& pluginName,
	                              const CppCoverage::CoverageData& coverageData,
	                              const std::wstring& argument) const
	{
		auto it = plugins_.find(pluginName);
		if (it == plugins_.end())
			THROW("Cannot find plugin: " << pluginName);
		auto& plugin = it->second;

		CallPluginfunction(
		    [&]() { plugin->Get().Export(coverageData, argument); },
		    "Export",
		    pluginFolder_ / pluginName);
	}
}
