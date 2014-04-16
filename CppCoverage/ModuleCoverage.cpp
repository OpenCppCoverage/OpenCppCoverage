#include "stdafx.h"
#include "ModuleCoverage.hpp"

#include "FileCoverage.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	ModuleCoverage::ModuleCoverage(const boost::filesystem::path& path)
		: path_(path)
	{
	}

	//-------------------------------------------------------------------------
	ModuleCoverage::~ModuleCoverage()
	{
	}

	//-------------------------------------------------------------------------
	FileCoverage& ModuleCoverage::AddFile(const boost::filesystem::path& filePath)
	{
		files_.push_back(std::unique_ptr<FileCoverage>(new FileCoverage(filePath)));

		return *files_.back();
	}

	//-------------------------------------------------------------------------
	const boost::filesystem::path& ModuleCoverage::GetPath() const
	{
		return path_;
	}

	//-------------------------------------------------------------------------
	const ModuleCoverage::T_FileCoverageCollection& ModuleCoverage::GetFiles() const
	{
		return files_;
	}

	//-------------------------------------------------------------------------
	void ModuleCoverage::ComputeCoverageRate()
	{		
		coverageRate_.RecursiveComputeFrom(files_);
		coverageRate_.SortByLowestRate(files_);
	}

	//-------------------------------------------------------------------------
	const CoverageRate& ModuleCoverage::GetCoverageRate() const
	{
		return coverageRate_;
	}
}
