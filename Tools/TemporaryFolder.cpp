#include "stdafx.h"
#include "TemporaryFolder.hpp"

#include "Log.hpp"

namespace fs = boost::filesystem;

namespace Tools
{
	//-------------------------------------------------------------------------	
	TemporaryFolder::TemporaryFolder()
	{
		path_ = fs::absolute(fs::temp_directory_path() / fs::unique_path());
	}

	//-------------------------------------------------------------------------
	TemporaryFolder::~TemporaryFolder()
	{
		boost::system::error_code error;
		if (!fs::remove_all(path_, error))
		{
			LOG_ERROR << error;
		}
	}

	//-------------------------------------------------------------------------
	TemporaryFolder::operator const boost::filesystem::path& () const
	{
		return GetPath();
	}

	//-------------------------------------------------------------------------
	const boost::filesystem::path& TemporaryFolder::GetPath() const
	{
		return path_;
	}
}
