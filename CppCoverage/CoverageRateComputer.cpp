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
#include "CoverageRateComputer.hpp"

#include "CoverageData.hpp"
#include "ModuleCoverage.hpp"
#include "FileCoverage.hpp"
#include "CoverageRate.hpp"

namespace CppCoverage
{
	namespace
	{
		//---------------------------------------------------------------------
		CoverageRate ComputeFileCoverage(const FileCoverage& file)
		{
			int executedLines = 0;
			int unexecutedLines = 0;

			for (const auto& lineCoverage : file.GetLines())
			{
				if (lineCoverage.HasBeenExecuted())
					++executedLines;
				else
					++unexecutedLines;
			}

			return CoverageRate{executedLines, unexecutedLines};
		}
		
		//---------------------------------------------------------------------
		template<typename Object>
		std::vector<Object*> SortByCoverageRate(
			const std::vector<std::unique_ptr<Object>>& objects,
			const std::unordered_map<const Object*, CoverageRate>& coverageRates)
		{
			std::vector<Object*> sortedObjects;
			
			for (const auto& object : objects)
				sortedObjects.push_back(object.get());

			std::sort(sortedObjects.begin(), sortedObjects.end(), 
				[&](const Object* object1, const Object* object2)
			{
				return coverageRates.at(object1).GetPercentRate() 
					 < coverageRates.at(object2).GetPercentRate();
			});

			return sortedObjects;
		}
	}

	//-------------------------------------------------------------------------
	CoverageRateComputer::CoverageRateComputer(const CoverageData& coverageData)
		: coverageData_(coverageData)
	{
		ComputeCoverageRateCache(coverageData);
	}
	
	//-------------------------------------------------------------------------
	void CoverageRateComputer::ComputeCoverageRateCache(const CoverageData& coverageData)
	{
		for (const auto& module : coverageData.GetModules())
		{
			CoverageRate moduleCoverageRate;

			for (const auto& file : module->GetFiles())
			{
				CoverageRate fileCoverageRate = ComputeFileCoverage(*file);

				moduleCoverageRate += fileCoverageRate;
				fileCoverageRate_.emplace(file.get(), fileCoverageRate);
			}

			moduleCoverageRate_.emplace(module.get(), moduleCoverageRate);
		}
	}
	
	//-------------------------------------------------------------------------
	std::vector<ModuleCoverage*> CoverageRateComputer::SortModulesByCoverageRate() const
	{
		return SortByCoverageRate(coverageData_.GetModules(), moduleCoverageRate_);
	}

	//-------------------------------------------------------------------------
	std::vector<FileCoverage*> CoverageRateComputer::SortFilesByCoverageRate(
		const ModuleCoverage& modules) const
	{
		return SortByCoverageRate(modules.GetFiles(), fileCoverageRate_);
	}

	//-------------------------------------------------------------------------
	const CoverageRate& CoverageRateComputer::GetCoverageRate(const ModuleCoverage& module) const
	{
		return moduleCoverageRate_.at(&module);
	}
	
	//-------------------------------------------------------------------------
	const CoverageRate& CoverageRateComputer::GetCoverageRate(const FileCoverage& file) const
	{
		return fileCoverageRate_.at(&file);
	}
}
