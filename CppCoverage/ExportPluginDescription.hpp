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

#include <string>
#include <functional>

namespace CppCoverage
{
	class ExportPluginDescription
	{
	  public:
		using CheckArgumentFct =
		    std::function<void(const std::optional<std::wstring>&)>;

		//-------------------------------------------------------------------------------
		ExportPluginDescription(std::wstring&& pluginName,
		                        std::wstring&& parameterDescription,
		                        CheckArgumentFct checkArgumentFct)
		    : pluginName_{std::move(pluginName)},
		      parameterDescription_{std::move(parameterDescription)},
		      checkArgumentFct_{checkArgumentFct}
		{
		}

		//-------------------------------------------------------------------------------
		ExportPluginDescription(const ExportPluginDescription&) = delete;
		ExportPluginDescription&
		operator=(const ExportPluginDescription&) = delete;
		ExportPluginDescription(ExportPluginDescription&&) = default;
		ExportPluginDescription& operator=(ExportPluginDescription&&) = default;

		//-------------------------------------------------------------------------------
		const std::wstring& GetPluginName() const
		{
			return pluginName_;
		}

		//-------------------------------------------------------------------------------
		const std::wstring& GetParameterDescription() const
		{
			return parameterDescription_;
		};

		//-------------------------------------------------------------------------------
		void CheckArgument(const std::optional<std::wstring>& parameter) const
		{
			checkArgumentFct_(parameter);
		}

	  private:
		std::wstring pluginName_;
		std::wstring parameterDescription_;
		CheckArgumentFct checkArgumentFct_;
	};
}
