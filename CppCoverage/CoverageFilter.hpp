#pragma once

#include <string>
#include <vector>
#include <memory>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CoverageSettings;
	class Patterns;

	class CPPCOVERAGE_DLL CoverageFilter
	{
	public:
		explicit CoverageFilter(const CoverageSettings&);
		~CoverageFilter();

		bool IsModuleSelected(const std::wstring& filename) const;
		bool IsSourceFileSelected(const std::wstring& filename) const;

	private:
		CoverageFilter(const CoverageFilter&) = delete;
		CoverageFilter& operator=(const CoverageFilter&) = delete;
		
		struct Filter;

		std::unique_ptr<Filter> BuildFilter(const Patterns& pattern) const;
		bool Match(
			const std::wstring& str,
			const Filter& filter,
			std::wostream& ostr) const;
	private:
		std::unique_ptr<Filter> moduleFilter_;
		std::unique_ptr<Filter> sourceFilter_;		
	};
}


