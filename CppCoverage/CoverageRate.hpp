#pragma once

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL CoverageRate
	{
	public:
		CoverageRate();

		template <typename T>
		void ComputeFrom(const std::vector<T>& values);

		void SetExecutedLinesCount(int);
		void SetUnexecutedLinesCount(int);

		int GetExecutedLinesCount() const;
		int GetUnexecutedLinesCount() const;
	
		CoverageRate& operator=(const CoverageRate&) = default;
		CoverageRate(const CoverageRate&) = default;
		
	private:
		int executedLinesCount_;
		int unexecutedLinesCount_;
	}; 

	//-------------------------------------------------------------------------
	template<typename T>
	void CoverageRate::ComputeFrom(const std::vector<T>& values)
	{		
		for (const auto& value : values)
		{
			const auto& coverageRate = value->GetCoverageRate();

			executedLinesCount_ += coverageRate.GetExecutedLinesCount();
			unexecutedLinesCount_ += coverageRate.GetUnexecutedLinesCount();
		}
	}
}

