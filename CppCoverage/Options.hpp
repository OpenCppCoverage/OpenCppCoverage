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
#include <filesystem>

#include "CppCoverageExport.hpp"
#include "Patterns.hpp"
#include "StartInfo.hpp"
#include "UnifiedDiffSettings.hpp"
#include "SubstitutePdbSourcePath.hpp"
#include "OptionsExport.hpp"

namespace CppCoverage
{
	class Patterns;	
	
	enum class LogLevel
	{
		Quiet,
		Normal,
		Verbose
	};

	class CPPCOVERAGE_DLL Options
	{
	public:
		Options(
			const Patterns& modulePatterns,
			const Patterns& sourcePatterns,
			const StartInfo*);
		
		Options(Options&&) = default;
		~Options();

		const Patterns& GetModulePatterns() const;
		const Patterns& GetSourcePatterns() const;
		const StartInfo* GetStartInfo() const;

		void SetLogLevel(LogLevel);
		LogLevel GetLogLevel() const;
		
		void EnablePlugingMode();
		bool IsPlugingModeEnabled() const;

		void EnableCoverChildrenMode();
		bool IsCoverChildrenModeEnabled() const;

        void EnableStopOnAssertMode();
        bool IsStopOnAssertModeEnabled() const;

		void EnableDumpOnCrash();
		bool IsDumpOnCrashEnabled() const;

		void DisableAggregateByFileMode();
		bool IsAggregateByFileModeEnabled() const;

		void EnableContinueAfterCppExceptionMode();
		bool IsContinueAfterCppExceptionModeEnabled() const;

		void AddExport(OptionsExport&&);
		const std::vector<OptionsExport>& GetExports() const;
		
		void AddInputCoveragePath(const std::filesystem::path&);
		const std::vector<std::filesystem::path>& GetInputCoveragePaths() const;

		void AddUnifiedDiffSettings(UnifiedDiffSettings&&);
		const std::vector<UnifiedDiffSettings>& GetUnifiedDiffSettingsCollection() const;

		void EnableOptimizedBuildSupport();
		bool IsOptimizedBuildSupportEnabled() const;

		void AddExcludedLineRegex(const std::wstring&);
		const std::vector<std::wstring>& GetExcludedLineRegexes() const;

		void AddSubstitutePdbSourcePath(SubstitutePdbSourcePath&&);
		const std::vector<SubstitutePdbSourcePath>& GetSubstitutePdbSourcePaths() const;

		friend CPPCOVERAGE_DLL std::wostream& operator<<(std::wostream&, const Options&);

	private:
		Options(const Options&) = delete;
		Options& operator=(Options&&) = delete;

	private:
		Patterns modules_;
		Patterns sources_;
		boost::optional<StartInfo> optionalStartInfo_;

		LogLevel logLevel_;
		bool isPluginModeEnabled_;
		bool isCoverChildrenModeEnabled_;
		bool isAggregateByFileModeEnabled_;
		bool isContinueAfterCppExceptionModeEnabled_;
        bool isStopOnAssertModeEnabled_;
		bool isDumpOnCrashEnabled_;
        bool isOptimizedBuildSupportEnabled_;
        std::vector<OptionsExport> exports_;
		std::vector<std::filesystem::path> inputCoveragePaths_;
		std::vector<UnifiedDiffSettings> unifiedDiffSettingsCollection_;
		std::vector<std::wstring> excludedLineRegexes_;
		std::vector<SubstitutePdbSourcePath> substitutePdbSourcePaths_;
	};
}