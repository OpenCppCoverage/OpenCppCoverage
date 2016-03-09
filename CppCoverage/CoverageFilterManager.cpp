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
	namespace
	{
		//---------------------------------------------------------------------
		template <typename Container, typename Fct>
		bool AnyOfOrTrueIfEmpty(const Container& container, Fct fct)
		{
			if (container.empty())
				return true;

			return std::any_of(container.begin(), container.end(), fct);
		}

		//---------------------------------------------------------------------
		CoverageFilterManager::UnifiedDiffCoverageFilters ToUnifiedDiffCoverageFilters(
					const std::vector<UnifiedDiffSettings>& unifiedDiffSettingsCollection)
		{
			CoverageFilterManager::UnifiedDiffCoverageFilters unifiedDiffCoverageFilters;

			for (const auto& unifiedDiffSettings : unifiedDiffSettingsCollection)
			{
				unifiedDiffCoverageFilters.emplace_back(
					std::make_unique<FileFilter::UnifiedDiffCoverageFilter>(
						unifiedDiffSettings.GetUnifiedDiffPath(), unifiedDiffSettings.GetDiffParentFolder()));
			}

			return unifiedDiffCoverageFilters;
		}
	}

	//-------------------------------------------------------------------------
	CoverageFilterManager::CoverageFilterManager(
		const CoverageSettings& settings,
		const std::vector<UnifiedDiffSettings>& unifiedDiffSettingsCollection)
		: CoverageFilterManager{ settings, ToUnifiedDiffCoverageFilters(unifiedDiffSettingsCollection) }
	{
	}

	//-------------------------------------------------------------------------
	CoverageFilterManager::CoverageFilterManager(
		const CoverageSettings& settings,
		UnifiedDiffCoverageFilters&& unifiedDiffCoverageFilters)
		: wildcardCoverageFilter_{ settings }
		, unifiedDiffCoverageFilters_( std::move(unifiedDiffCoverageFilters) )
	{
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

		return AnyOfOrTrueIfEmpty(unifiedDiffCoverageFilters_, [&](const auto& filter) {
			return filter->IsSourceFileSelected(filename);
		});
	}

	//-------------------------------------------------------------------------
	bool CoverageFilterManager::IsLineSelected(const std::wstring& filename, int lineNumber)
	{
		return AnyOfOrTrueIfEmpty(unifiedDiffCoverageFilters_, [&](const auto& filter) {
			return filter->IsLineSelected(filename, lineNumber);
		});
	}
}
