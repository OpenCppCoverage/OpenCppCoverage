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

#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	enum class OptionsExportType
	{
		Html,
		Cobertura
	};

	class CPPCOVERAGE_DLL OptionsExport
	{
	public:
		explicit OptionsExport(OptionsExportType type);
		OptionsExport(OptionsExportType type, const boost::filesystem::path&);

		OptionsExport(const OptionsExport&) = default;
		OptionsExport& operator=(const OptionsExport&) = default;
		
		OptionsExportType GetType() const;
		const std::wstring& GetTypeString() const;
		const boost::optional<boost::filesystem::path>& GetOutputPath() const;

		friend std::wostream& operator<<(std::wostream& ostr, const OptionsExport&);

	private:
		OptionsExportType type_;
		boost::optional<boost::filesystem::path> outputPath_;
	};
}

