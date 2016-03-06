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
#include "CoverageFilterManager.hpp"
#include "UnifiedDiffSettings.hpp"

#include "FileFilter/UnifiedDiffCoverageFilter.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	CoverageFilterManager::CoverageFilterManager(
		const CoverageSettings& settings,
		const UnifiedDiffSettings* unifiedDiffSettings)
		: wildcardCoverageFilter_{ settings }
	{
		if (unifiedDiffSettings)
		{
			unifiedDiffCoverageFilter_ =
				std::make_unique<FileFilter::UnifiedDiffCoverageFilter>(
					unifiedDiffSettings->GetUnifiedDiffPath(), unifiedDiffSettings->GetDiffParentFolder());
		}
	}

	//-------------------------------------------------------------------------
	CoverageFilterManager::~CoverageFilterManager() = default;

	//-------------------------------------------------------------------------
	bool CoverageFilterManager::IsModuleSelected(const std::wstring& filename) const
	{
		return wildcardCoverageFilter_.IsModuleSelected(filename);
	}

	//-------------------------------------------------------------------------
	bool CoverageFilterManager::IsSourceFileSelected(const std::wstring& filename) const
	{
		if (!wildcardCoverageFilter_.IsSourceFileSelected(filename))
			return false;

		return !unifiedDiffCoverageFilter_ || unifiedDiffCoverageFilter_->IsSourceFileSelected(filename);
	}

	//-------------------------------------------------------------------------
	bool CoverageFilterManager::IsLineSelected(const std::wstring& filename, int lineNumber) const
	{
		return !unifiedDiffCoverageFilter_
			|| unifiedDiffCoverageFilter_->IsLineSelected(filename, lineNumber);
	}
}
