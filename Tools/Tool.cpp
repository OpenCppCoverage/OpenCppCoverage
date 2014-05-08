#include "stdafx.h"
#include "Tool.hpp"

#include <cvt/wstring>
#include <codecvt>

#include "Log.hpp"
#include "ToolsException.hpp"

namespace fs = boost::filesystem;

namespace Tools
{
	namespace
	{
		//-----------------------------------------------------------------------------
		fs::path GetExecutablePath()
		{
			std::vector<wchar_t> filename(4096);

			if (!GetModuleFileName(nullptr, &filename[0], filename.size()))
				THROW("Cannot get current executable path.");

			return fs::path{ &filename[0] };
		}

		//-----------------------------------------------------------------------------
		fs::path GetExecutableFolder()
		{
			fs::path executablePath = GetExecutablePath();

			return executablePath.parent_path();
		}
	}

	//-------------------------------------------------------------------------
	std::string ToString(const std::wstring& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

		return converter.to_bytes(str);
	}

	//-------------------------------------------------------------------------
	std::wstring ToWString(const std::string& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		
		return converter.from_bytes(str);
	}
		
	//-------------------------------------------------------------------------
	boost::filesystem::path GetTemplateFolder()
	{
		return GetExecutableFolder() / "Template";
	}
		
	//-------------------------------------------------------------------------
	boost::filesystem::path GetUniquePath(const boost::filesystem::path& prefix)
	{
		std::string uniquePath = prefix.string();

		for (int i = 2; fs::exists(uniquePath); ++i)
			uniquePath = prefix.string() + '-' + std::to_string(i);

		return uniquePath;
	}

	//-------------------------------------------------------------------------
	void Try(std::function<void()> action)
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

	//-------------------------------------------------------------------------
	void CheckPathExists(const fs::path& path)
	{
		if (!fs::exists(path))
			THROW(path.wstring() << L" does not exist");
	}

}


