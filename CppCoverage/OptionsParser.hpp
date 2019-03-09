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
#include <map>
#include <memory>

#include "CppCoverageExport.hpp"

namespace Tools
{
	class WarningManager;
}

namespace boost
{
	namespace program_options
	{
		class variables_map;
	}
}

namespace CppCoverage
{
	class Options;
	class ProgramOptions;
	enum class OptionsExportType;
	class OptionsExport;
	class ProgramOptionsVariablesMap;

	class CPPCOVERAGE_DLL OptionsParser
	{
	public:
		static const char ExportSeparator;
		static const char PathSeparator;
		static const int DosCommandLineMaxSize;

		OptionsParser();
		explicit OptionsParser(std::shared_ptr<Tools::WarningManager>);
		~OptionsParser();

		boost::optional<Options> Parse(int argc, const char** argv, std::wostream* emptyOptionsExplanation) const;
				
		static std::wstring GetTooLongCommandLineMessage();

	private:
		OptionsParser(const OptionsParser&) = delete;
		OptionsParser& operator=(const OptionsParser&) = delete;
		
		boost::optional<Options> Parse(int argc, const char** argv) const;
		void ShowExplanation(std::wostream* emptyOptionsExplanation, const char* message) const;
		
		void AddExporTypes(
			const ProgramOptionsVariablesMap&,
			Options& options) const;

		OptionsExport CreateExport(const std::string&) const;

		std::map<std::string, OptionsExportType> exportTypes_;
		std::unique_ptr<ProgramOptions> programOptions_;
		std::shared_ptr<Tools::WarningManager> optionalWarningManager_;
	};
}

