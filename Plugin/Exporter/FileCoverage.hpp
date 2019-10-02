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
#include <map>

#include "LineCoverage.hpp"
#include "../PluginExport.hpp"

namespace Plugin
{
	class PLUGIN_DLL FileCoverage
	{
	public:
		explicit FileCoverage(const std::filesystem::path& path);

		void AddLine(unsigned int lineNumber, bool hasBeenExecuted);
		void UpdateLine(unsigned int lineNumber, bool hasBeenExecuted);

		const std::filesystem::path& GetPath() const;
		const LineCoverage* operator[](unsigned int line) const;
		std::vector<LineCoverage> GetLines() const;

		FileCoverage& operator=(const FileCoverage&) = default;

	private:
		FileCoverage(const FileCoverage&) = delete;
			
	private:
		std::filesystem::path path_;
		std::map<unsigned int, LineCoverage> lines_;	
	};
}


