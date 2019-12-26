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
#include "CoverageData.hpp"

#include "ModuleCoverage.hpp"

namespace Plugin
{
	//-------------------------------------------------------------------------
	CoverageData::CoverageData(const std::wstring& name, int exitCode)
		: name_(name)
		, exitCode_(exitCode)
	{
	}

	//-------------------------------------------------------------------------
	CoverageData::~CoverageData()
	{
	}

	//-------------------------------------------------------------------------
	CoverageData::CoverageData(CoverageData&& coverageData)
	{
		*this = std::move(coverageData);
	}

	//-------------------------------------------------------------------------
	CoverageData& CoverageData::operator=(CoverageData&& coverageData)
	{
		if (this != &coverageData)
		{
			std::swap(modules_, coverageData.modules_);
			name_ = coverageData.name_;
			exitCode_ = coverageData.exitCode_;
		}
		return *this;
	}

	//-------------------------------------------------------------------------
	ModuleCoverage& CoverageData::AddModule(const std::filesystem::path& path)
	{
		modules_.push_back(std::unique_ptr<ModuleCoverage>(new ModuleCoverage(path)));

		return *modules_.back();
	}

	//-------------------------------------------------------------------------	
	void CoverageData::SetName(const std::wstring& name)
	{
		name_ = name;
	}

	//-------------------------------------------------------------------------
	void CoverageData::SetExitCode(int exitCode)
	{
		exitCode_ = exitCode;
	}

	//-------------------------------------------------------------------------
	const CoverageData::T_ModuleCoverageCollection& CoverageData::GetModules() const
	{
		return modules_;
	}
	
	//-------------------------------------------------------------------------	
	const std::wstring& CoverageData::GetName() const
	{
		return name_;
	}
		
	//-------------------------------------------------------------------------	
	int CoverageData::GetExitCode() const
	{
		return exitCode_;
	}
}

