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
#include <boost/filesystem.hpp>

#include "CppCoverageExport.hpp"
#include "CppCoverage/Patterns.hpp"
#include "CppCoverage/StartInfo.hpp"

namespace CppCoverage
{
	class Patterns;

	enum class OptionsExportType;

	class CPPCOVERAGE_DLL Options
	{
	public:
		Options(
			const CppCoverage::StartInfo& startInfo,
			const CppCoverage::Patterns& modulePatterns,
			const CppCoverage::Patterns& sourcePatterns);

		Options(const Options&) = default;
		
		const CppCoverage::Patterns& GetModulePatterns() const;
		const CppCoverage::Patterns& GetSourcePatterns() const;
		const CppCoverage::StartInfo& GetStartInfo() const;

		void SetVerboseModeSelected();
		bool IsVerboseModeSelected() const;

		void AddExportType(OptionsExportType);
		const std::vector<OptionsExportType>& GetExportTypes() const;

		void SetOutputDirectoryOption(const boost::filesystem::path&);
		const boost::optional<boost::filesystem::path>& 
			GetOutputDirectoryOption() const;

		friend CPPCOVERAGE_DLL std::wostream& operator<<(std::wostream&, const Options&);

	private:
		Options(Options&&) = delete;
		Options& operator=(Options&&) = delete;

	private:
		CppCoverage::StartInfo startInfo_;
		CppCoverage::Patterns modules_;
		CppCoverage::Patterns sources_;
		bool verboseModeSelected_;
		std::vector<OptionsExportType> exportTypes_;
		boost::optional<boost::filesystem::path> outputDirectoryOption_;
	};
}