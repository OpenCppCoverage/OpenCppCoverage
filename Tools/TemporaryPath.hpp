#pragma once

#include <boost/filesystem.hpp>

#include "ToolsExport.hpp"

namespace Tools
{
	class TOOLS_DLL TemporaryPath
	{
	public:
		TemporaryPath();
		~TemporaryPath();

		operator const boost::filesystem::path& () const;
		const boost::filesystem::path& GetPath() const;

	private:
		TemporaryPath(const TemporaryPath&) = delete;
		TemporaryPath& operator=(const TemporaryPath&) = delete;

	private:
		boost::filesystem::path path_;
	};
}