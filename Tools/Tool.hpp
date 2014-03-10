#pragma once

#include <string>

#include "ToolsExport.hpp"

namespace Tools
{
	class TOOLS_DLL Tool
	{
	public:
		Tool() = delete;

		static std::wstring ToWString(const std::string&);
		static std::string ToString(const std::wstring&);
	};
}


