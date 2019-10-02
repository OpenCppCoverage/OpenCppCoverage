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

#include "IPluginLoader.hpp"
#include "LoadedPlugin.hpp"

#include "Tools/Tool.hpp"

#include <Windows.h>

namespace Exporter
{
	template <typename T>
	class PluginLoader : public IPluginLoader<T>
	{
	  public:
		//-------------------------------------------------------------------------
		std::unique_ptr<LoadedPlugin<T>>
		TryLoadPlugin(const std::filesystem::path& pluginPath,
		              const std::string& pluginFactoryFctName) const override
		{
			auto libModule = LoadLibrary(pluginPath.c_str());

			if (!libModule)
				throw std::runtime_error("LoadLibrary failed.");

			auto plugin = std::make_unique<LoadedPlugin<T>>(libModule);

			auto address =
			    GetProcAddress(libModule, pluginFactoryFctName.c_str());
			if (!address)
				throw std::runtime_error("Cannot find C function " +
				                         pluginFactoryFctName + '.');
			auto pluginFactory = reinterpret_cast<T* (*)()>(address);

			T* rawPlugin = Tools::Try<std::runtime_error>(
			    [&]() { return pluginFactory(); },
			    [&](const auto& error) {
				    return "Error when calling " + pluginFactoryFctName + ": " +
				           error;
			    });

			if (!rawPlugin)
				throw std::runtime_error(pluginFactoryFctName +
				                         " returns null.");

			plugin->Set(std::unique_ptr<T>{rawPlugin});

			return plugin;
		}
	};
}
