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
#include <optional>

namespace Plugin
{
	class CoverageData;

	//-------------------------------------------------------------------------
	// This is the interface to implement a new export type.
	//-------------------------------------------------------------------------
	class IExportPlugin
	{
	  public:
		virtual ~IExportPlugin() = default;

		//---------------------------------------------------------------------
		// Perform the export.
		//    coverageData: stores the result of the code coverage.
		//    argument: The command line argument provided by the user or std::nullopt.
		//---------------------------------------------------------------------
		virtual void Export(const Plugin::CoverageData& coverageData,
		                    const std::optional<std::wstring>& argument) = 0;

		//---------------------------------------------------------------------
		// Check the command line argument.
		//    argument: The command line argument provided by the user or std::nullopt.
		// If the argument is not valid, this function must throw an instance of
		// OptionsParserException.
		//---------------------------------------------------------------------
		virtual void CheckArgument(const std::optional<std::wstring>& argument) = 0;
	
		//---------------------------------------------------------------------
		// Get the text to describe the command line argument.
		// For example, it can be "output file (optional)".
		//---------------------------------------------------------------------
		virtual std::wstring GetArgumentHelpDescription() = 0; 
	};
}
