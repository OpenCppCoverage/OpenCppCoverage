// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage
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

#include <boost/algorithm/string.hpp>

#include "CppCoverageException.hpp"
#include "CoverageLevel.hpp"
#include "CoverageFilterManager.hpp"
#include "UnifiedDiffCoverageFilterManager.hpp"

#include "FileFilter/FileInfo.hpp"
#include "FileFilter/LineInfo.hpp"
#include "FileFilter/ReleaseCoverageFilter.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	CoverageFilterManager::CoverageFilterManager(
		const CoverageFilterSettings& settings,	
		const std::vector<UnifiedDiffSettings>& unifiedDiffSettingsCollection,
		const std::vector<std::wstring>& excludedLineRegexes,
		bool useReleaseCoverageFilter,
		CoverageLevel coverageLevel)
		: wildcardCoverageFilter_{ settings }
		, unifiedDiffCoverageFilterManager_{ unifiedDiffSettingsCollection }
		, lineFilter_{ excludedLineRegexes }
		, optionalReleaseCoverageFilter_{ useReleaseCoverageFilter ?
			std::make_unique<FileFilter::ReleaseCoverageFilter>() : nullptr }
		, coverageLevel_{ coverageLevel }
	{
		if (coverageLevel != CoverageLevel::Line && coverageLevel != CoverageLevel::Source)
			THROW("Invalid coverage level");
	}

	//-------------------------------------------------------------------------
	CoverageFilterManager::~CoverageFilterManager() = default;

	//-------------------------------------------------------------------------
	bool CoverageFilterManager::IsModuleSelected(const std::wstring& filename) const
	{
		return wildcardCoverageFilter_.IsModuleSelected(filename);
	}

	//-------------------------------------------------------------------------
	bool CoverageFilterManager::IsSourceFileSelected(const std::wstring& filename)
	{
		if (!wildcardCoverageFilter_.IsSourceFileSelected(filename))
			return false;

		return unifiedDiffCoverageFilterManager_.IsSourceFileSelected(filename);
	}

	//-------------------------------------------------------------------------
	CoverageLevel CoverageFilterManager::GetCoverageLevel() const
	{
		return coverageLevel_;
	}

	//-------------------------------------------------------------------------
	bool CoverageFilterManager::IsLineSelected(
		const FileFilter::ModuleInfo& moduleInfo,
		const FileFilter::FileInfo& fileInfo,
		const FileFilter::LineInfo& lineInfo)
	{
		if (optionalReleaseCoverageFilter_ &&
			!optionalReleaseCoverageFilter_->IsLineSelected(moduleInfo, fileInfo, lineInfo))
		{
			return false;
		}

		if (!lineFilter_.IsLineSelected(fileInfo, lineInfo))
			return false;

		return unifiedDiffCoverageFilterManager_.IsLineSelected(fileInfo, lineInfo);
	}

	//-------------------------------------------------------------------------
	std::vector<std::wstring> CoverageFilterManager::ComputeWarningMessageLines(size_t maxUnmatchPaths) const
	{
		return unifiedDiffCoverageFilterManager_.ComputeWarningMessageLines(maxUnmatchPaths);
	}
}
