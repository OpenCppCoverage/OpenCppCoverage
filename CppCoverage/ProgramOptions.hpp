// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <boost/program_options.hpp>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL ProgramOptions
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
		static const std::string ConfigFileOption;
		static const std::string WorkingDirectoryOption;
		static const std::string OutputDirectoryOption;
		static const std::string ProgramToRunOption;
		static const std::string ProgramToRunArgOption;

		ProgramOptions();

		void ProgramOptions::FillVariableMap(
			int argc,
			const char** argv,
			boost::program_options::variables_map& variables) const;

		void FillVariableMap(std::istream&, boost::program_options::variables_map& variables) const;

		friend CPPCOVERAGE_DLL std::wostream& operator<<(std::wostream&, const ProgramOptions&);

	private:
		ProgramOptions(const ProgramOptions&) = delete;
		ProgramOptions& operator=(const ProgramOptions&) = delete;

		boost::program_options::options_description genericOptions_;
		boost::program_options::options_description configurationOptions_;
		boost::program_options::options_description hiddenOptions_;

		boost::program_options::options_description commandLineOptions_;
		boost::program_options::options_description visibleOptions_;
		boost::program_options::options_description configFileOptions_;
		boost::program_options::positional_options_description positionalOptions_;
	};
}