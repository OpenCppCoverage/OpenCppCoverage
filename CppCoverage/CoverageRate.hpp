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

#pragma once

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL CoverageRate
	{
	public:
		CoverageRate();

		template <typename T>
		void RecursiveComputeFrom(std::vector<T>& values);

		template<typename T>
		void SortByLowestRate(std::vector<T>& values) const;

		void SetExecutedLinesCount(int);
		void SetUnexecutedLinesCount(int);

		int GetExecutedLinesCount() const;
		int GetUnExecutedLinesCount() const;
		int GetTotalLinesCount() const;
		int GetPercentRate() const;

		CoverageRate& operator=(const CoverageRate&) = default;
		CoverageRate(const CoverageRate&) = default;

	private:
		template <typename T>
		static int GetPercentRate(const T& value);

	private:
		int executedLinesCount_;
		int unexecutedLinesCount_;
	}; 

	//-------------------------------------------------------------------------
	template <typename T>
	int CoverageRate::GetPercentRate(const T& value)
	{
		const auto& coverageRate = value->GetCoverageRate();

		return coverageRate.GetPercentRate();
	}

	//-------------------------------------------------------------------------
	template<typename T>
	void CoverageRate::RecursiveComputeFrom(std::vector<T>& values)
	{		
		executedLinesCount_ = 0;
		unexecutedLinesCount_ = 0;

		for (auto& value : values)
		{
			value->ComputeCoverageRate();

			const auto& coverageRate = value->GetCoverageRate();

			executedLinesCount_ += coverageRate.executedLinesCount_;
			unexecutedLinesCount_ += coverageRate.unexecutedLinesCount_;
		}		
	}

	//-------------------------------------------------------------------------
	template<typename T>
	void CoverageRate::SortByLowestRate(std::vector<T>& values) const
	{
		std::sort(begin(values), end(values), [this](const T& value1, const T& value2)
		{
			return GetPercentRate(value1) < GetPercentRate(value2);
		});
	}
}

