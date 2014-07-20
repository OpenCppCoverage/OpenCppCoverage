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

#include <boost/optional.hpp>
#include <iosfwd>

#include "CppCoverageExport.hpp"
#include "ProgramOptions.hpp"

namespace CppCoverage
{
	class Options;

	class CPPCOVERAGE_DLL OptionsParser
	{
	public:
		OptionsParser() = default;

		boost::optional<Options> Parse(int argc, const char** argv, std::wostream* emptyOptionsExplanation) const;
				
	private:
		OptionsParser(const OptionsParser&) = delete;
		OptionsParser& operator=(const OptionsParser&) = delete;
		
		boost::optional<Options> Parse(int argc, const char** argv) const;
		void ShowExplanation(std::wostream* emptyOptionsExplanation, const char* message) const;
		
		ProgramOptions programOptions_;		
	};
}

