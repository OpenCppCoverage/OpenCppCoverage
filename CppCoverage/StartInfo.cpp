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
#include "StartInfo.hpp"

#include <boost/filesystem.hpp>

#include "Tools/Tool.hpp"
#include "CppCoverageException.hpp"

namespace fs = boost::filesystem;

namespace CppCoverage
{	
	//-------------------------------------------------------------------------
	StartInfo::StartInfo(const fs::path& path)
		: path_(path)
	{
		Tools::CheckPathExists(path);
		AddArguments(path.wstring());
	}

	//-------------------------------------------------------------------------
	StartInfo::StartInfo(StartInfo&& startInfo)
		: path_{ std::move(startInfo.path_) }
		, arguments_{ std::move(startInfo.arguments_) }
		, workingDirectory_{ std::move(startInfo.workingDirectory_) }
	{
	}

	//-------------------------------------------------------------------------
	void StartInfo::SetWorkingDirectory(const fs::path& workingDirectory)
	{
		Tools::CheckPathExists(workingDirectory);
		workingDirectory_ = workingDirectory;					
	}

	//-------------------------------------------------------------------------
	void StartInfo::AddArguments(const std::wstring& argument)
	{
		arguments_.push_back(argument);
	}

	//-------------------------------------------------------------------------
	const boost::filesystem::path& StartInfo::GetPath() const
	{
		return path_;
	}

	//-------------------------------------------------------------------------
	const std::vector<std::wstring>& StartInfo::GetArguments() const
	{
		return arguments_;
	}

	//-------------------------------------------------------------------------
	const fs::path* StartInfo::GetWorkingDirectory() const
	{
		if (workingDirectory_)
			return &workingDirectory_.get();
		return nullptr;
	}

	//-------------------------------------------------------------------------
	std::wostream& operator<<(std::wostream& ostr, const StartInfo& startInfo)
	{
		ostr << L"Path:" << startInfo.path_ << std::endl;
		ostr << L"Arguments:";
		
		for (const auto& arg : startInfo.arguments_)
			ostr << arg << " ";
		ostr << std::endl;
		ostr << L"Working directory:";
		
		if (startInfo.workingDirectory_)
			ostr << startInfo.workingDirectory_;
		else
			ostr << " not set.";

		ostr << std::endl;
		return ostr;
	}
}
