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
#include <iostream>

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	OptionsExport::OptionsExport(OptionsExportType type,
	                             std::wstring&& name,
	                             std::optional<std::wstring>&& parameter)
	    : type_{type}, name_{std::move(name)}, parameter_{std::move(parameter)}
	{
	}

	//-------------------------------------------------------------------------
	OptionsExportType OptionsExport::GetType() const
	{
		return type_;
	}

	//-------------------------------------------------------------------------
	const std::wstring& OptionsExport::GetName() const
	{
		return name_;
	}

	//-------------------------------------------------------------------------
	const std::optional<std::wstring>& OptionsExport::GetParameter() const
	{
		return parameter_;
	}

	//-------------------------------------------------------------------------
	std::wostream& operator<<(std::wostream& ostr,
	                          const OptionsExport& optionsExport)
	{
		ostr << optionsExport.GetName();

		auto parameter = optionsExport.GetParameter();

		if (parameter)
			ostr << " " << *parameter;
		return ostr;
	}
}
