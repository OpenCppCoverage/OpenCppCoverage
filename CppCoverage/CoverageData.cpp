#include "stdafx.h"
#include "CoverageData.hpp"

#include "ModuleCoverage.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	CoverageData::CoverageData(const std::wstring& name)
		: name_(name)
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
		coverageRate_ = coverageData.coverageRate_;
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
	
	//-------------------------------------------------------------------------	
	const std::wstring& CoverageData::GetName() const
	{
		return name_;
	}

	//-------------------------------------------------------------------------	
	const CoverageRate& CoverageData::GetCoverageRate() const
	{
		return coverageRate_;
	}

	//-------------------------------------------------------------------------	
	void CoverageData::ComputeCoverageRate()
	{
		coverageRate_.RecursiveComputeFrom(modules_);
		coverageRate_.SortByLowestRate(modules_);
	}
}

