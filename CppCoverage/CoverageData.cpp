#include "stdafx.h"
#include "CoverageData.hpp"

#include "ModuleCoverage.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	CoverageData::CoverageData()
	{
	}

	//-------------------------------------------------------------------------
	CoverageData::~CoverageData()
	{
	}

	//-------------------------------------------------------------------------
	CoverageData::CoverageData(CoverageData&& coverageData)
	{
		std::swap(modules_, coverageData.modules_);
	}

	//-------------------------------------------------------------------------
	ModuleCoverage& CoverageData::AddModule(const std::wstring& name)
	{
		modules_.push_back(std::unique_ptr<ModuleCoverage>(new ModuleCoverage(name)));

		return *modules_.back();
	}

	//-------------------------------------------------------------------------	
	const CoverageData::T_ModuleCoverageCollection& CoverageData::GetModules() const
	{
		return modules_;
	}
}

