// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2018 OpenCppCoverage
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
#include "InvalidOutputFileException.hpp"

namespace Exporter
{
	namespace
	{
		//---------------------------------------------------------------------
		std::string GetErrorMessage(const std::filesystem::path& output,
		                            const std::string& outputKind)
		{
			auto error = "Cannot write " + outputKind +
			             " export to the file: \"" + output.string() + "\".";

			if (std::filesystem::is_directory(output))
			{
				error += " The output \"" + output.string() +
				         "\" is a directory whereas as it must be a file.";
			}
			return error;
		}
	}

	//-------------------------------------------------------------------------
	InvalidOutputFileException::InvalidOutputFileException(
	    const std::filesystem::path& output, const std::string& outputKind)
	    : std::runtime_error(GetErrorMessage(output, outputKind))
	{
	}
}
