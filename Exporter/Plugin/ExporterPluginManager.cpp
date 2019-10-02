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

#include <optional>

#include "Plugin/OptionsParserException.hpp"

#include "IPluginLoader.hpp"
#include "LoadedPlugin.hpp"
#include "Plugin/Exporter/IExportPlugin.hpp"
#include "../ExporterException.hpp"
#include "Tools/Tool.hpp"

namespace Exporter
{
	namespace
	{
		//---------------------------------------------------------------------
		std::string
		InvalidPluginError(const std::optional<std::string>& functionName,
		                   const std::string& additionalInformation,
		                   const std::filesystem::path& pluginPath)
		{
			std::string fullMessage =
			    "There was an error when trying to load the export plugin " +
			    pluginPath.string() + '\n';

			if (functionName)
				fullMessage += "Error: Function " + *functionName + " failed: ";
			fullMessage += additionalInformation;
			fullMessage +=
			    "\nRemoving " + pluginPath.string() + " may fix the problem.";

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
			return Tools::Try<std::runtime_error>(
			    [&]() { return pluginFct(std::forward<Args>(args)...); },
			    [&](const auto& error) {
				    return InvalidPluginError(functionName, error, pluginPath);
			    });
		}

		//---------------------------------------------------------------------
		void CheckVersion(const Plugin::IExportPlugin& exportPlugin,
		                  const std::filesystem::path& pluginPath)
		{
			const auto functionName = "GetExportPluginVersion";
			auto pluginVersion = CallPluginfunction(
			    [&]() { return exportPlugin.GetExportPluginVersion(); },
			    functionName,
			    pluginPath);
			auto currentVersion = Plugin::CurrentExportPluginVersion;
			if (pluginVersion != currentVersion)
			{
				auto error =
				    "IExportPlugin version missmatch: "
				    "OpenCppCoverage version is " +
				    std::to_string(currentVersion) +
				    " whereas the plugin version is " + std::to_string(pluginVersion);
				throw std::runtime_error(
				    InvalidPluginError(functionName, error, pluginPath));
			}
		}
	}

	//-------------------------------------------------------------------------
	ExporterPluginManager::ExporterPluginManager(
	    const IPluginLoader<Plugin::IExportPlugin>& pluginLoader,
	    std::filesystem::path&& pluginFolder)
	    : pluginFolder_{std::move(pluginFolder)}
	{
		for (auto pluginPath :
		     std::filesystem::directory_iterator(pluginFolder_))
		{
			auto path = pluginPath.path();

			if (path.extension() != ".dll")
				continue;

			auto pluginName = path.stem().wstring();
			const std::string pluginFactoryFctName = "CreatePlugin";

			auto plugin = Tools::Try<std::runtime_error>(
			    [&]() {
				    return pluginLoader.TryLoadPlugin(pluginPath,
				                                      pluginFactoryFctName);
			    },
			    [&](const auto& error) {
				    return InvalidPluginError(std::nullopt, error, path);
			    });
			CheckVersion(plugin->Get(), path);
			plugins_.emplace(pluginName, std::move(plugin));
		}
	}

	//-------------------------------------------------------------------------
	ExporterPluginManager::~ExporterPluginManager() = default;

	//-------------------------------------------------------------------------
	void
	CheckArgument(const std::weak_ptr<LoadedPlugin<Plugin::IExportPlugin>>& weakPlugin,
	              const std::filesystem::path& pluginPath,
	              const std::optional<std::wstring>& parameter)
	{
		auto p = weakPlugin.lock();
		if (!p)
			THROW("Plugin was released");

		std::string error;
		try
		{
			p->Get().CheckArgument(parameter);
			return;
		}
		catch (const Plugin::OptionsParserException&)
		{
			throw;
		}
		catch (const std::exception& e)
		{
			error = e.what();
		}
		catch (...)
		{
			error = "Unknow";
		}

		auto fullErrorMessage = InvalidPluginError("CheckArgument", error, pluginPath);
		throw std::runtime_error(fullErrorMessage);
	}

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
			    [&]() { return exportPlugin->Get().GetArgumentHelpDescription(); },
			    "GetHelpDescription",
			    pluginPath);

			std::weak_ptr<LoadedPlugin<Plugin::IExportPlugin>> weakPlugin{exportPlugin};

			exportPluginDescriptions.push_back(
			    CppCoverage::ExportPluginDescription{
			        std::wstring{pluginName},
			        std::move(helpDescription),
			        [weakPlugin, pluginPath, this](
			            const std::optional<std::wstring>& parameter) {
					CheckArgument(weakPlugin, pluginPath, parameter);
				}});
		}

		return exportPluginDescriptions;
	}

	//-------------------------------------------------------------------------
	void
	ExporterPluginManager::Export(const std::wstring& pluginName,
	                              const Plugin::CoverageData& coverageData,
	                              const std::optional<std::wstring>& argument) const
	{
		auto it = plugins_.find(pluginName);
		if (it == plugins_.end())
			THROW("Cannot find plugin: " << pluginName);
		auto& plugin = it->second;

		auto optionalOutput = CallPluginfunction(
		    [&]() { return plugin->Get().Export(coverageData, argument); },
		    "Export",
		    pluginFolder_ / pluginName);
		if (optionalOutput)
		{
			Tools::ShowOutputMessage(
			    pluginName + L" has generated the report at ", *optionalOutput);
		}
	}
}
