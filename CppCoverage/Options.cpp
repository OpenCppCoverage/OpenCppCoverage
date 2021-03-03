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

#include "stdafx.h"
#include "Options.hpp"
#include "CppCoverageException.hpp"
#include "OptionsExport.hpp"

namespace CppCoverage
{
	namespace
	{
		//---------------------------------------------------------------------
		std::wstring GetLogLevelStr(LogLevel logLevel)
		{
			switch (logLevel)
			{
			case LogLevel::Normal: return L"Normal";
			case LogLevel::Quiet: return L"Quiet";
			case LogLevel::Verbose: return L"Verbose";
			}
			THROW("Invalid Log level.");
		}
	}

	//-------------------------------------------------------------------------
	Options::Options(
		const Patterns& modulePatterns,
		const Patterns& sourcePatterns,
		const StartInfo* startInfo)
		: modules_{modulePatterns}
		, sources_{sourcePatterns}		
		, logLevel_{ LogLevel::Normal }
		, isPluginModeEnabled_{false}
		, isCoverChildrenModeEnabled_{false}
		, isAggregateByFileModeEnabled_{true}
		, isContinueAfterCppExceptionModeEnabled_{false}
		, isStopOnAssertModeEnabled_{false}
		, isDumpOnCrashEnabled_{false}
		, isOptimizedBuildSupportEnabled_{false}
	{
		if (startInfo)
			optionalStartInfo_ = *startInfo;
	}	

	//-------------------------------------------------------------------------
	Options::~Options() = default;

	//-------------------------------------------------------------------------
	const Patterns& Options::GetModulePatterns() const
	{
		return modules_;
	}
	
	//-------------------------------------------------------------------------
	const Patterns& Options::GetSourcePatterns() const
	{
		return sources_;
	}
	
	//-------------------------------------------------------------------------
	const StartInfo* Options::GetStartInfo() const
	{
		return optionalStartInfo_.get_ptr();
	}

	//-------------------------------------------------------------------------
	void Options::SetLogLevel(LogLevel logLevel)
	{
		logLevel_ = logLevel;
	}

	//-------------------------------------------------------------------------
	LogLevel Options::GetLogLevel() const
	{
		return logLevel_;
	}

	//-------------------------------------------------------------------------
	void Options::EnablePlugingMode()
	{
		isPluginModeEnabled_ = true;
	}

	//-------------------------------------------------------------------------
	bool Options::IsPlugingModeEnabled() const
	{
		return isPluginModeEnabled_;
	}

	//-------------------------------------------------------------------------
	void Options::EnableCoverChildrenMode()
	{
		isCoverChildrenModeEnabled_ = true;
	}

	//-------------------------------------------------------------------------
	bool Options::IsCoverChildrenModeEnabled() const
	{
		return isCoverChildrenModeEnabled_;
	}

	//-------------------------------------------------------------------------
	void Options::DisableAggregateByFileMode()
	{
		isAggregateByFileModeEnabled_ = false;
	}

	//-------------------------------------------------------------------------
	bool Options::IsAggregateByFileModeEnabled() const
	{
		return isAggregateByFileModeEnabled_;
	}

	//-------------------------------------------------------------------------
	void Options::EnableContinueAfterCppExceptionMode()
	{
		isContinueAfterCppExceptionModeEnabled_ = true;
	}
	
    //-------------------------------------------------------------------------
	bool Options::IsContinueAfterCppExceptionModeEnabled() const
	{
		return isContinueAfterCppExceptionModeEnabled_;
	}

    //-------------------------------------------------------------------------
    void Options::EnableStopOnAssertMode()
    {
      isStopOnAssertModeEnabled_ = true;
    }
    
    //-------------------------------------------------------------------------
    bool Options::IsStopOnAssertModeEnabled() const
    {
      return isStopOnAssertModeEnabled_;
    }

    //-------------------------------------------------------------------------
    void Options::EnableDumpOnCrash()
    {
      isDumpOnCrashEnabled_ = true;
    }
    
    //-------------------------------------------------------------------------
    bool Options::IsDumpOnCrashEnabled() const
    {
      return isDumpOnCrashEnabled_;
    }

    //-------------------------------------------------------------------------
	void Options::AddExport(OptionsExport&& optionExport)
	{
		exports_.push_back(std::move(optionExport));
	}
	
	//-------------------------------------------------------------------------
	const std::vector<OptionsExport>& Options::GetExports() const
	{
		return exports_;
	}

	//-------------------------------------------------------------------------
	void Options::AddInputCoveragePath(const std::filesystem::path& path)
	{
		inputCoveragePaths_.push_back(path);
	}

	//-------------------------------------------------------------------------
	const std::vector<std::filesystem::path>& Options::GetInputCoveragePaths() const
	{
		return inputCoveragePaths_;
	}

	//-------------------------------------------------------------------------
	void Options::AddUnifiedDiffSettings(UnifiedDiffSettings&& unifiedDiffSettings)
	{
		unifiedDiffSettingsCollection_.push_back(std::move(unifiedDiffSettings));
	}

	//-------------------------------------------------------------------------
	const std::vector<UnifiedDiffSettings>& Options::GetUnifiedDiffSettingsCollection() const
	{
		return unifiedDiffSettingsCollection_;
	}

	//-------------------------------------------------------------------------
	void Options::EnableOptimizedBuildSupport()
	{
		isOptimizedBuildSupportEnabled_ = true;
	}

	//-------------------------------------------------------------------------
	bool Options::IsOptimizedBuildSupportEnabled() const
	{
		return isOptimizedBuildSupportEnabled_;
	}

	//-------------------------------------------------------------------------
	void Options::AddExcludedLineRegex(const std::wstring& excludedRegex)
	{
		excludedLineRegexes_.push_back(excludedRegex);
	}

	//-------------------------------------------------------------------------
	const std::vector<std::wstring>& Options::GetExcludedLineRegexes() const
	{
		return excludedLineRegexes_;
	}

	//-------------------------------------------------------------------------
	void Options::AddSubstitutePdbSourcePath(SubstitutePdbSourcePath&& substitutePdbSourcePath)
	{
		substitutePdbSourcePaths_.push_back(std::move(substitutePdbSourcePath));
	}

	//-------------------------------------------------------------------------
	const std::vector<SubstitutePdbSourcePath>& Options::GetSubstitutePdbSourcePaths() const
	{
		return substitutePdbSourcePaths_;
	}

	//-------------------------------------------------------------------------
	std::wostream& operator<<(std::wostream& ostr, const Options& options)
	{
		if (options.optionalStartInfo_)
			ostr << *options.optionalStartInfo_ << std::endl;
		ostr << L"Modules: " << options.modules_ << std::endl;
		ostr << L"Sources: " << options.sources_ << std::endl;
		ostr << L"Log Level: " << GetLogLevelStr(options.GetLogLevel()) << std::endl;
		ostr << L"Cover Children: " << options.isCoverChildrenModeEnabled_ << std::endl;
		ostr << L"Aggregate by file: " << options.isAggregateByFileModeEnabled_ << std::endl;
		ostr << L"Continue after C++ exception: " << options.isContinueAfterCppExceptionModeEnabled_ << std::endl;
		ostr << L"Create minidump on crash: " << options.isDumpOnCrashEnabled_ << std::endl;
		ostr << L"Optimized build support: " << options.isOptimizedBuildSupportEnabled_ << std::endl;

		ostr << L"Export: ";
		for (const auto& optionExport : options.exports_)
			ostr << optionExport << L" ";
		ostr << std::endl;

		ostr << L"Input coverage: ";
		for (const auto& path : options.inputCoveragePaths_)
			ostr << path.wstring() << L" ";
		ostr << std::endl;

		ostr << L"Unified diff: ";
		for (const auto& settings : options.unifiedDiffSettingsCollection_)
		{
			ostr << settings.GetUnifiedDiffPath() << L" Root folder: ";
			ostr << settings.GetRootDiffFolder().get_value_or(L"") << " ";
		}
		ostr << std::endl;

		ostr << L"Excluded line regular expressions: ";
		for (const auto& excludedRegex : options.excludedLineRegexes_)
			ostr << excludedRegex << L" ";
		ostr << std::endl;

		ostr << L"Substitute pdb source paths: ";
		for (const auto& substitutePdbSourcePath : options.substitutePdbSourcePaths_)
		{
			ostr << substitutePdbSourcePath.GetPdbStartPath() << L" => ";
			ostr << substitutePdbSourcePath.GetLocalPath() << "  ";
		}
		ostr << std::endl;

		return ostr;
	}
}
