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

#include "../ExporterException.hpp"

#include <Windows.h>

namespace Exporter
{
	template <typename T>
	class LoadedPlugin
	{
	  public:
		//---------------------------------------------------------------------
		explicit LoadedPlugin(HMODULE libModule) : libModule_{libModule}
		{
		}

		//---------------------------------------------------------------------
		~LoadedPlugin()
		{
			plugin_.reset();
			if (libModule_)
				FreeLibrary(libModule_);
		}

		//---------------------------------------------------------------------
		void Set(std::unique_ptr<T> plugin)
		{
			plugin_ = std::move(plugin);
		}

		//---------------------------------------------------------------------
		T& Get() const
		{
			if (!plugin_)
				THROW("Null plugin");
			return *plugin_;
		}

	  private:
		const HMODULE libModule_;
		std::unique_ptr<T> plugin_;
	};
}
