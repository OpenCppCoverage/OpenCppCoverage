#pragma once

#include <string>
#include <functional>

#include <boost/filesystem.hpp>

#include "ToolsExport.hpp"

namespace Tools
{	
	TOOLS_DLL std::wstring ToWString(const std::string&);
	TOOLS_DLL std::string ToString(const std::wstring&);
	TOOLS_DLL void Try(std::function<void()>);
	TOOLS_DLL void CheckPathExists(const boost::filesystem::path&);		
	TOOLS_DLL boost::filesystem::path GetTemplateFolder();

	TOOLS_DLL boost::filesystem::path MakeRelativeFrom(
		const boost::filesystem::path& absolutePath, 
		const boost::filesystem::path& currentFolder);

	TOOLS_DLL boost::filesystem::path GetUniquePath(const boost::filesystem::path& prefix);
}


