// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2017 OpenCppCoverage
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
#include "RunCoverageSettings.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	RunCoverageSettings::RunCoverageSettings(
	    const StartInfo& startInfo,
	    const CoverageFilterSettings& settings,
	    const std::vector<UnifiedDiffSettings>& unifiedDiffSettings,
	    const std::vector<std::wstring>& excludedLineRegexes,
	    const std::vector<SubstitutePdbSourcePath>& substitutePdbSourcePath)
	    : startInfo_{startInfo},
	      coverageFilterSettings_{settings},
	      unifiedDiffSettings_{unifiedDiffSettings},
	      coverChildren_{false},
	      continueAfterCppException_{false},
	      stopOnAssert_{false},
	      dumpOnCrash_{false},
	      maxUnmatchPathsForWarning_{0},
	      optimizedBuildSupport_{false},
	      excludedLineRegexes_{excludedLineRegexes},
	      substitutePdbSourcePath_{substitutePdbSourcePath}
	{
	}

	//-------------------------------------------------------------------------
	void RunCoverageSettings::SetCoverChildren(bool coverChildren)
	{
		coverChildren_ = coverChildren;
	}
	
	//-------------------------------------------------------------------------
	void RunCoverageSettings::SetContinueAfterCppException(bool continueAfterCppException)
	{
		continueAfterCppException_ = continueAfterCppException;
	}
	
    //-------------------------------------------------------------------------
    void RunCoverageSettings::SetStopOnAssert(bool stopOnAssert)
    {
      stopOnAssert_ = stopOnAssert;
    }

	//-------------------------------------------------------------------------
	void RunCoverageSettings::SetDumpOnCrash(bool dumpOnCrash)
	{
		dumpOnCrash_ = dumpOnCrash;
	}

    //-------------------------------------------------------------------------
	void RunCoverageSettings::SetMaxUnmatchPathsForWarning(size_t maxUnmatchPathsForWarning)
	{
		maxUnmatchPathsForWarning_ = maxUnmatchPathsForWarning;
	}

	//-------------------------------------------------------------------------
	void RunCoverageSettings::SetOptimizedBuildSupport(bool optimizedBuildSupport)
	{
		optimizedBuildSupport_ = optimizedBuildSupport;
	}

	//-------------------------------------------------------------------------
	const StartInfo& RunCoverageSettings::GetStartInfo() const
	{
		return startInfo_;
	}
	
	//-------------------------------------------------------------------------
	const CoverageFilterSettings& RunCoverageSettings::GetCoverageFilterSettings() const
	{
		return coverageFilterSettings_;
	}
	
	//-------------------------------------------------------------------------
	const std::vector<UnifiedDiffSettings>& RunCoverageSettings::GetUnifiedDiffSettings() const
	{
		return unifiedDiffSettings_;
	}
	
	//-------------------------------------------------------------------------
	bool RunCoverageSettings::GetCoverChildren() const
	{
		return coverChildren_;
	}
	
	//-------------------------------------------------------------------------
	bool RunCoverageSettings::GetContinueAfterCppException() const
	{
		return continueAfterCppException_;
	}
	
    //-------------------------------------------------------------------------
    bool RunCoverageSettings::GetStopOnAssert() const
    {
      return stopOnAssert_;
    }
    
	//-------------------------------------------------------------------------
	bool RunCoverageSettings::GetDumpOnCrash() const
	{
		return dumpOnCrash_;
	}

	//-------------------------------------------------------------------------
	size_t RunCoverageSettings::GetMaxUnmatchPathsForWarning() const
	{
		return maxUnmatchPathsForWarning_;
	}

	//-------------------------------------------------------------------------
	bool RunCoverageSettings::GetOptimizedBuildSupport() const
	{
		return optimizedBuildSupport_;
	}

	//-------------------------------------------------------------------------
	const std::vector<std::wstring>& RunCoverageSettings::GetExcludedLineRegexes() const
	{
		return excludedLineRegexes_;
	}

	//-------------------------------------------------------------------------
	const std::vector<SubstitutePdbSourcePath>& RunCoverageSettings::GetSubstitutePdbSourcePaths() const
	{
		return substitutePdbSourcePath_;
	}
}
