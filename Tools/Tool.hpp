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

#include <string>
#include <functional>

#include <filesystem>
#include <boost/optional/optional_fwd.hpp>

#include "ToolsExport.hpp"

namespace Tools
{	
	TOOLS_DLL std::wstring LocalToWString(const std::string&);
	TOOLS_DLL std::wstring Utf8ToWString(const std::string&);
	TOOLS_DLL std::string ToLocalString(const std::wstring&);
	TOOLS_DLL std::string ToUtf8String(const std::wstring&);

	TOOLS_DLL boost::optional<std::wstring> Try(std::function<void()>);	
	TOOLS_DLL std::filesystem::path GetExecutableFolder();
	
	TOOLS_DLL void CreateMiniDumpOnUnHandledException();

	TOOLS_DLL void ShowOutputMessage(const std::wstring& message, const std::filesystem::path& path);
	TOOLS_DLL std::wstring GetSeparatorLine();

	TOOLS_DLL void CreateParentFolderIfNeeded(const std::filesystem::path& path);
}


