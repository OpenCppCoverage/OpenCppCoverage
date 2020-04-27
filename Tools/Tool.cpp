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

#include "stdafx.h"
#include "Tool.hpp"

#include <boost/optional/optional.hpp>
#include <cvt/wstring>
#include <codecvt>
#include <filesystem>
#include <system_error>

#include "Log.hpp"
#include "ToolsException.hpp"

namespace fs = std::filesystem;

namespace Tools
{
	namespace
	{
		//-----------------------------------------------------------------------------
		fs::path GetExecutablePath()
		{
			const int PathBufferSize = 40 * 1000;
			std::vector<wchar_t> filename(PathBufferSize);

			if (!GetModuleFileName(nullptr, &filename[0], static_cast<int>(filename.size())))
				THROW("Cannot get current executable path.");

			return fs::path{ &filename[0] };
		}
		
		//-------------------------------------------------------------------------
		std::string ToString(unsigned int pageCode, const std::wstring& str)
		{
			if (str.empty())
				return{};

			auto size = WideCharToMultiByte(pageCode, 0, str.c_str(),
				static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
			std::vector<char> buffer(size);

			if (!WideCharToMultiByte(pageCode, 0, str.c_str(), static_cast<int>(str.size()),
				&buffer[0], static_cast<int>(buffer.size()), nullptr, nullptr))
			{
				throw std::runtime_error("Error in WideCharToMultiByte.");
			}

			return{ buffer.begin(), buffer.end() };
		}

		//-------------------------------------------------------------------------
		std::wstring ToWString(unsigned int pageCode, const std::string& str)
		{
			if (str.empty())
				return{};

			auto size = MultiByteToWideChar(pageCode, 0,
				str.c_str(), static_cast<int>(str.size()), nullptr, 0);
			std::vector<wchar_t> buffer(size);

			if (!MultiByteToWideChar(pageCode, 0, str.c_str(), static_cast<int>(str.size()),
				&buffer[0], static_cast<int>(buffer.size())))
			{
				throw std::runtime_error("Error in MultiByteToWideChar for " + str);
			}

			return{ buffer.begin(), buffer.end() };
		}
	}

	//-----------------------------------------------------------------------------
	fs::path GetExecutableFolder()
	{
		fs::path executablePath = GetExecutablePath();

		return executablePath.parent_path();
	}

	//-------------------------------------------------------------------------
	std::string ToLocalString(const std::wstring& str)
	{
		return ToString(CP_ACP, str);
	}

	//-------------------------------------------------------------------------
	std::string ToUtf8String(const std::wstring& str)
	{
		return ToString(CP_UTF8, str);
	}

	//-------------------------------------------------------------------------
	std::wstring LocalToWString(const std::string& str)
	{
		return ToWString(CP_ACP, str);
	}

	//-------------------------------------------------------------------------
	std::wstring Utf8ToWString(const std::string& str)
	{
		return ToWString(CP_UTF8, str);
	}

	//-------------------------------------------------------------------------
	boost::optional<std::wstring> Try(std::function<void()> action)
	{
		try
		{
			action();
		}
		catch (const std::exception& e)
		{
			return Tools::LocalToWString(e.what());
		}
		catch (...)
		{
			return std::wstring{L"Unkown exception"};
		}

		return boost::none;
	}

	//-------------------------------------------------------------------------
	void ShowOutputMessage(
		const std::wstring& message, 
		const std::filesystem::path& path)
	{
		LOG_INFO << GetSeparatorLine();
		LOG_INFO << message << path.wstring();
		LOG_INFO << GetSeparatorLine();
	}

	//-------------------------------------------------------------------------
	std::wstring GetSeparatorLine()
	{
		return L"----------------------------------------------------";
	}
	
	//---------------------------------------------------------------------
	void CreateParentFolderIfNeeded(const std::filesystem::path& path)
	{
		if (path.has_parent_path())
		{
			auto parentPath = path.parent_path();
			std::error_code er;

			std::filesystem::create_directories(parentPath, er);
			if (er)
			{
				THROW(L"Error when creating folder " << parentPath.wstring()
					<< L" Error code:" << er.value());
			}
		}
	}

	//---------------------------------------------------------------------
	bool FileExists(const std::filesystem::path& path)
	{
		std::error_code ignoredErrorCode;

		// Error can happen when the drive is not ready (DVD for example).
		return std::filesystem::exists(path, ignoredErrorCode);
	}
}