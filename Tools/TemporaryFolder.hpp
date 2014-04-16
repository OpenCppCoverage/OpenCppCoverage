#pragma once

#include <boost/filesystem.hpp>

#include "ToolsExport.hpp"

namespace Tools
{
	class TOOLS_DLL TemporaryFolder
	{
	public:
		TemporaryFolder();
		~TemporaryFolder();

		operator const boost::filesystem::path& () const;
		const boost::filesystem::path& GetPath() const;

	private:
		TemporaryFolder(const TemporaryFolder&) = delete;
		TemporaryFolder& operator=(const TemporaryFolder&) = delete;

	private:
		boost::filesystem::path path_;
	};
}