#include "stdafx.h"
#include "Tool.hpp"
#include "Log.hpp"

namespace fs = boost::filesystem;

namespace Tools
{
	//-------------------------------------------------------------------------
	std::string Tool::ToString(const std::wstring& str)
	{
		return std::string(str.begin(), str.end());
	}

	//-------------------------------------------------------------------------
	std::wstring Tool::ToWString(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path Tool::GetUniquePath(const boost::filesystem::path& prefix)
	{
		std::string uniquePath = prefix.string();

		for (int i = 2; fs::exists(uniquePath); ++i)
			uniquePath = prefix.string() + '-' + std::to_string(i);

		return uniquePath;
	}

	//-------------------------------------------------------------------------
	void Tool::Try(std::function<void()> action)
	{
		try
		{
			action();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "ERROR: " << e.what();
		}
		catch (...)
		{
			LOG_ERROR << "Unkown exception";
		}
	}

}


