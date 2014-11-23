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

#include "stdafx.h"
#include "OptionsExport.hpp"

#include <map>

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	OptionsExport::OptionsExport(OptionsExportType type)
		: type_{type}
	{
	}

	//-------------------------------------------------------------------------
	OptionsExport::OptionsExport(
		OptionsExportType type, 
		const boost::filesystem::path& outputPath)
		: type_{type}
		, outputPath_{ outputPath }
	{
	}

	//-------------------------------------------------------------------------
	OptionsExportType OptionsExport::GetType() const
	{
		return type_;
	}

	//-------------------------------------------------------------------------
	const std::wstring& OptionsExport::GetTypeString() const
	{
		static const std::map<OptionsExportType, std::wstring> optionsExportTypeTexts =
		{ { OptionsExportType::Html, L"Html" },
		{ OptionsExportType::Cobertura, L"Cobertura" },
		{ OptionsExportType::Binary, L"Binary" } };

		return optionsExportTypeTexts.at(type_);
	}

	//-------------------------------------------------------------------------
	const boost::optional<boost::filesystem::path>& OptionsExport::GetOutputPath() const
	{
		return outputPath_;
	}

	//-------------------------------------------------------------------------
	std::wostream& operator<<(
		std::wostream& ostr,
		const OptionsExport& optionsExport)
	{
		ostr << optionsExport.GetTypeString();

		auto outputPath = optionsExport.GetOutputPath();

		if (outputPath)
			ostr << " " << outputPath->wstring();
		return ostr;
	}
}
