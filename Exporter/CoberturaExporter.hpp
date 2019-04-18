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

#include <iosfwd>
#include <filesystem>

#include "ExporterExport.hpp"
#include "IExporter.hpp"

namespace CppCoverage
{
	class CoverageData;
}

namespace Exporter
{
	class EXPORTER_DLL CoberturaExporter: public IExporter
	{
	public:
		CoberturaExporter() ;

		std::filesystem::path GetDefaultPath(const std::wstring& runningCommandFilename) const override;
		void Export(const CppCoverage::CoverageData&, const std::filesystem::path& output) override;
		void Export(const CppCoverage::CoverageData&, std::wostream&) const;

	private:
		CoberturaExporter(const CoberturaExporter&) = delete;
		CoberturaExporter& operator=(const CoberturaExporter&) = delete;
	};
}

