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

#include <vector>
#include <string>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL Patterns
	{
	public:
		typedef std::vector<std::wstring> T_Patterns;

	public:
		explicit Patterns(bool isRegexCaseSensitiv = false);
		Patterns(const Patterns&) = default;
		Patterns(Patterns&&);

		void AddSelectedPatterns(const std::wstring&);
		const T_Patterns& GetSelectedPatterns() const;

		void AddExcludedPatterns(const std::wstring&);
		const T_Patterns& GetExcludedPatterns() const;

		bool IsRegexCaseSensitiv() const;

		Patterns& operator=(const Patterns&) = default;
		
		CPPCOVERAGE_DLL friend std::wostream& operator<<(std::wostream&, const Patterns&);
	
	private:
		T_Patterns selectedPatterns_;
		T_Patterns excludedPatterns_;
		bool isRegexCaseSensitiv_;
	};
}

