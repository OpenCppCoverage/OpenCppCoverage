// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "../ExporterExport.hpp"
#include "../IExporter.hpp"

namespace Exporter
{
	class EXPORTER_DLL BinaryExporter : public IExporter
	{
	public:
		BinaryExporter() = default;

		std::filesystem::path GetDefaultPath(const std::wstring& prefix) const override;
		void Export(const Plugin::CoverageData&, const std::filesystem::path& output) override;

	private:
		BinaryExporter(const BinaryExporter&) = delete;
		BinaryExporter& operator=(const BinaryExporter&) = delete;
	};
}

