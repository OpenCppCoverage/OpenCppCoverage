#pragma once

#include <string>
#include <functional>

#include <boost/filesystem.hpp>

#include "ToolsExport.hpp"

namespace Tools
{
	class TOOLS_DLL Tool
	{
	public:
		Tool() = delete;

		static std::wstring ToWString(const std::string&);
		static std::string ToString(const std::wstring&);
		static void Try(std::function<void()>);

		static boost::filesystem::path GetUniquePath(const boost::filesystem::path& prefix);
	};
}


