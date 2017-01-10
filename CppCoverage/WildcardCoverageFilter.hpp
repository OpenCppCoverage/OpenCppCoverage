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

#include <string>
#include <vector>
#include <memory>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CoverageFilterSettings;
	class Patterns;

	class CPPCOVERAGE_DLL WildcardCoverageFilter
	{
	public:
		explicit WildcardCoverageFilter(const CoverageFilterSettings&);
		~WildcardCoverageFilter();

		bool IsModuleSelected(const std::wstring& filename) const;
		bool IsSourceFileSelected(const std::wstring& filename) const;

	private:
		WildcardCoverageFilter(const WildcardCoverageFilter&) = delete;
		WildcardCoverageFilter& operator=(const WildcardCoverageFilter&) = delete;
		
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


