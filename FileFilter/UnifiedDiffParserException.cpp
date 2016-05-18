// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage
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
#include "UnifiedDiffParserException.hpp"
#include "UnifiedDiffParser.hpp"
#include "Tools/Tool.hpp"

namespace FileFilter
{
	//-------------------------------------------------------------------------
	const std::wstring UnifiedDiffParserException::ErrorContextHunks
		= L"Context line count is not consistent with hunks differences.";
	const std::wstring UnifiedDiffParserException::ErrorNoFilenameBeforeHunks
		= L"No filename previously found.";
	const std::wstring UnifiedDiffParserException::ErrorCannotReadLine
		= L"Cannot read a new line. Expect a line starting with " + UnifiedDiffParser::ToFilePrefix + L'.';
	const std::wstring UnifiedDiffParserException::ErrorExpectFromFilePrefix
		= L"Invalid line. Expect a line starting with " + UnifiedDiffParser::ToFilePrefix + L'.';
	const std::wstring UnifiedDiffParserException::ErrorInvalidHunks = L"Invalid hunks differences.";

	//-------------------------------------------------------------------------
	UnifiedDiffParserException::UnifiedDiffParserException(const std::wstring& message)
		: std::runtime_error(Tools::ToLocalString(message))
	{}
}
