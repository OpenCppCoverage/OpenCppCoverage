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

#include <optional>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	enum class OptionsExportType
	{
		Html,
		Cobertura,
		Binary,
		Plugin
	};

	class CPPCOVERAGE_DLL OptionsExport
	{
	  public:
		explicit OptionsExport(OptionsExportType type,
		                       std::wstring&& text,
		                       std::optional<std::wstring>&& argument);

		OptionsExport(OptionsExport&&) = default;
		OptionsExport& operator=(OptionsExport&&) = default;

		OptionsExport(const OptionsExport&) = delete;
		OptionsExport& operator=(const OptionsExport&) = delete;
		
		OptionsExportType GetType() const;
		const std::wstring& GetText() const;
		const std::optional<std::wstring>& GetParameter() const;

		friend std::wostream& operator<<(std::wostream& ostr,
		                                 const OptionsExport&);

	  private:
		OptionsExportType type_;
		std::wstring text_;
		std::optional<std::wstring> parameter_;
	};
}
