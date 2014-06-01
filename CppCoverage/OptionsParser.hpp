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

#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#include <iosfwd>

namespace CppCoverage
{
	class Options;

	class CPPCOVERAGE_DLL OptionsParser
	{
	public:
		static const std::string SelectedModulesOption;
		static const std::string ExcludedModulesOption;
		static const std::string SelectedSourcesOption;
		static const std::string ExcludedSourcesOption;
		static const std::string VerboseOption;
		static const std::string VerboseShortOption;
		static const std::string HelpOption;
		static const std::string HelpShortOption;
		static const std::string WorkingDirectoryOption;
		static const std::string OutputDirectoryOption;

	public:
		OptionsParser();

		boost::optional<Options> Parse(int argc, const char** argv) const;
		
		friend CPPCOVERAGE_DLL std::wostream& operator<<(std::wostream&, const OptionsParser&);

	private:
		OptionsParser(const OptionsParser&) = delete;
		OptionsParser& operator=(const OptionsParser&) = delete;

		void FillVariableMap(int argc, const char** argv, boost::program_options::variables_map& variables) const;

	private:
		boost::program_options::options_description description_;
	};
}

