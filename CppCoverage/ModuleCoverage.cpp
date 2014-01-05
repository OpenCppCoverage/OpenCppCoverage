#include "stdafx.h"
#include "ModuleCoverage.hpp"

#include "FileCoverage.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	ModuleCoverage::ModuleCoverage(const std::wstring& name)
		: name_(name)
	{
	}

	//-------------------------------------------------------------------------
	ModuleCoverage::~ModuleCoverage()
	{
	}

	//-------------------------------------------------------------------------
	FileCoverage& ModuleCoverage::AddFile(const std::wstring& filename)
	{
		files_.push_back(std::unique_ptr<FileCoverage>(new FileCoverage(filename)));

		return *files_.back();
	}

	//-------------------------------------------------------------------------
	const std::wstring& ModuleCoverage::GetName() const
	{
		return name_;
	}

	//-------------------------------------------------------------------------
	const ModuleCoverage::T_FileCoverageCollection& ModuleCoverage::GetFiles() const
	{
		return files_;
	}
}
