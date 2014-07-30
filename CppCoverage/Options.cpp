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
#include "Options.hpp"

namespace cov = CppCoverage;

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	Options::Options(
		const cov::StartInfo& startInfo,
		const cov::Patterns& modulePatterns,
		const cov::Patterns& sourcePatterns)
		: startInfo_{startInfo}
		, modules_{modulePatterns}
		, sources_{sourcePatterns}
		, verboseModeSelected_{false}
	{
	}	

	//-------------------------------------------------------------------------
	const cov::Patterns& Options::GetModulePatterns() const
	{
		return modules_;
	}
	
	//-------------------------------------------------------------------------
	const cov::Patterns& Options::GetSourcePatterns() const
	{
		return sources_;
	}
	
	//-------------------------------------------------------------------------
	const cov::StartInfo& Options::GetStartInfo() const
	{
		return startInfo_;
	}

	//-------------------------------------------------------------------------
	void Options::SetVerboseModeSelected()
	{
		verboseModeSelected_ = true;
	}

	//-------------------------------------------------------------------------
	bool Options::IsVerboseModeSelected() const
	{
		return verboseModeSelected_;
	}

	//-------------------------------------------------------------------------
	void Options::SetOutputDirectoryOption(const boost::filesystem::path& outputDirectoryOption)
	{
		outputDirectoryOption_ = outputDirectoryOption;
	}

	//-------------------------------------------------------------------------
	const boost::optional<boost::filesystem::path>& 
		Options::GetOutputDirectoryOption() const
	{
		return outputDirectoryOption_;
	}

	//-------------------------------------------------------------------------
	void Options::AddExportType(OptionsExportType type)
	{
		exportTypes_.push_back(type);
	}
	
	//-------------------------------------------------------------------------
	const std::vector<OptionsExportType>& Options::GetExportTypes() const
	{
		return exportTypes_;
	}

	//-------------------------------------------------------------------------
	std::wostream& operator<<(std::wostream& ostr, const Options& options)
	{
		ostr << options.startInfo_ << std::endl;
		ostr << L"Modules: " << options.modules_ << std::endl;
		ostr << L"Sources: " << options.sources_ << std::endl;
		ostr << L"Verbose mode: " << options.verboseModeSelected_ << std::endl;

		return ostr;
	}
}
