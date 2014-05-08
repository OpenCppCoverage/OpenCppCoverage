#include "stdafx.h"
#include "TemporaryPath.hpp"

#include "Log.hpp"

namespace fs = boost::filesystem;

namespace Tools
{
	//-------------------------------------------------------------------------	
	TemporaryPath::TemporaryPath()
	{
		path_ = fs::absolute(fs::temp_directory_path() / fs::unique_path());
	}

	//-------------------------------------------------------------------------
	TemporaryPath::~TemporaryPath()
	{
		boost::system::error_code error;
		if (!fs::remove_all(path_, error))
		{
			LOG_ERROR << error;
		}
	}

	//-------------------------------------------------------------------------
	TemporaryPath::operator const boost::filesystem::path& () const
	{
		return GetPath();
	}

	//-------------------------------------------------------------------------
	const boost::filesystem::path& TemporaryPath::GetPath() const
	{
		return path_;
	}
}
