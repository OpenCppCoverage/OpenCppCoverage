// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2017 OpenCppCoverage
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
#include "MappedFile.hpp"

#include "ToolsException.hpp"

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem/operations.hpp>

namespace Tools
{
	//-------------------------------------------------------------------------
	MappedFile::MappedFile(const boost::filesystem::path& path)
	{
		boost::iostreams::mapped_file mappedFile(path.string(), boost::iostreams::mapped_file::readonly);

		if (!mappedFile)
			THROW(L"Cannot create mapped file: " + path.wstring());

		auto begin = mappedFile.const_begin();
		const auto end = mappedFile.const_end();
		for (auto it = begin; it != end; ++it)
		{
			if (*it == '\n')
			{
				const auto endOfLine = (it != begin && *(it - 1) == '\r') ? it - 1 : it;
				lines_.push_back({ begin, endOfLine });
				begin = it + 1;
			}
		}
		if (begin != end)
			lines_.push_back({begin, end});
	}

	//-------------------------------------------------------------------------
	const std::vector<std::string>& MappedFile::GetLines() const
	{
		return lines_;
	}

	//-------------------------------------------------------------------------
	std::unique_ptr<MappedFile> MappedFile::TryCreate(const boost::filesystem::path& path)
	{
		if (!boost::filesystem::exists(path) || boost::filesystem::file_size(path) == 0)
			return nullptr;
		return std::unique_ptr<MappedFile>(new MappedFile{ path });
	}
}
