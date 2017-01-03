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

#include "UnifiedDiffCoverageFilterManager.hpp"
#include "UnifiedDiffSettings.hpp"
#include "ProgramOptions.hpp"
#include "FileFilter/UnifiedDiffCoverageFilter.hpp"
#include "FileFilter/FileInfo.hpp"
#include "FileFilter/LineInfo.hpp"

#include "Tools/Tool.hpp"
#include "Tools/Log.hpp"

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
		UnifiedDiffCoverageFilterManager::UnifiedDiffCoverageFilters ToUnifiedDiffCoverageFilters(
					const std::vector<UnifiedDiffSettings>& unifiedDiffSettingsCollection)
		{
			UnifiedDiffCoverageFilterManager::UnifiedDiffCoverageFilters unifiedDiffCoverageFilters;

			for (const auto& unifiedDiffSettings : unifiedDiffSettingsCollection)
			{
				unifiedDiffCoverageFilters.emplace_back(
					std::make_unique<FileFilter::UnifiedDiffCoverageFilter>(
						unifiedDiffSettings.GetUnifiedDiffPath(), unifiedDiffSettings.GetRootDiffFolder()));
			}

			return unifiedDiffCoverageFilters;
		}

		//-------------------------------------------------------------------------
		boost::optional<int> GetExecutableLineOrPreviousOne(
			int lineNumber,
			const std::set<int>& executableLinesSet)
		{
			auto it = executableLinesSet.lower_bound(lineNumber);

			if (it != executableLinesSet.end() && *it == lineNumber)
				return lineNumber;

			return (it == executableLinesSet.begin()) ? boost::optional<int>{} : *(--it);
		}
	}

	//-------------------------------------------------------------------------
	UnifiedDiffCoverageFilterManager::UnifiedDiffCoverageFilterManager(
		const std::vector<UnifiedDiffSettings>& unifiedDiffSettingsCollection)
		: UnifiedDiffCoverageFilterManager{
			ToUnifiedDiffCoverageFilters(unifiedDiffSettingsCollection)}
	{
	}

	//-------------------------------------------------------------------------
	UnifiedDiffCoverageFilterManager::UnifiedDiffCoverageFilterManager(
		UnifiedDiffCoverageFilters&& unifiedDiffCoverageFilters)
		: unifiedDiffCoverageFilters_( std::move(unifiedDiffCoverageFilters) )
	{
	}

	//-------------------------------------------------------------------------
	UnifiedDiffCoverageFilterManager::~UnifiedDiffCoverageFilterManager() = default;

	//-------------------------------------------------------------------------
	bool UnifiedDiffCoverageFilterManager::IsSourceFileSelected(const std::wstring& filename)
	{
		return AnyOfOrTrueIfEmpty(unifiedDiffCoverageFilters_, [&](const auto& filter) {
			return filter->IsSourceFileSelected(filename);
		});
	}

	//-------------------------------------------------------------------------
	bool UnifiedDiffCoverageFilterManager::IsLineSelected(
		const FileFilter::FileInfo& fileInfo,
		const FileFilter::LineInfo& lineInfo)
	{
		if (unifiedDiffCoverageFilters_.empty())
			return true;

		const auto& executableLinesSet = GetExecutableLinesSet(fileInfo);

		auto executableLineNumber = GetExecutableLineOrPreviousOne(lineInfo.lineNumber_, executableLinesSet);
		if (!executableLineNumber)
			return false;

		return AnyOfOrTrueIfEmpty(unifiedDiffCoverageFilters_, [&](const auto& filter) {
			return filter->IsLineSelected(fileInfo.filePath_, *executableLineNumber);
		});
	}

	//-------------------------------------------------------------------------
	std::vector<std::wstring> UnifiedDiffCoverageFilterManager::ComputeWarningMessageLines(size_t maxUnmatchPaths) const
	{
		std::set<boost::filesystem::path> unmatchPaths;

		for (const auto& filter : unifiedDiffCoverageFilters_)
		{
			auto paths = filter->GetUnmatchedPaths();
			unmatchPaths.insert(paths.begin(), paths.end());
		}

		return ComputeWarningMessageLines(unmatchPaths, maxUnmatchPaths);
	}

	//-------------------------------------------------------------------------
	std::vector<std::wstring> UnifiedDiffCoverageFilterManager::ComputeWarningMessageLines(
		const std::set<boost::filesystem::path>& unmatchPaths,
		size_t maxUnmatchPaths) const
	{
		std::vector<std::wstring> messageLines;
		if (!unmatchPaths.empty())
		{
			messageLines.push_back(Tools::GetSeparatorLine());
			messageLines.push_back(L"You have " + std::to_wstring(unmatchPaths.size())
				+ L" path(s) inside unified diff file(s) that were ignored");
			messageLines.push_back(L"because they did not match any path from pdb files.");
			messageLines.push_back(L"To see all files use --" + 
				Tools::LocalToWString(ProgramOptions::VerboseOption));

			size_t i = 0;
			for (const auto& path : unmatchPaths)
			{
				if (i++ >= maxUnmatchPaths)
				{
					messageLines.push_back(L"\t...");
					break;
				}
				messageLines.push_back(L"\t- " + path.wstring());
			}
		}

		return messageLines;
	}

	//-------------------------------------------------------------------------
	const std::set<int>& UnifiedDiffCoverageFilterManager::GetExecutableLinesSet(
		const FileFilter::FileInfo& fileInfo)
	{
		auto filePath = fileInfo.filePath_;

		if (filePath != executableLineCache_.currentFilePath)
		{
			auto& executableLinesSet = executableLineCache_.executableLinesSet;

			executableLinesSet.clear();
			for (const auto& lineInfo : fileInfo.lineInfoColllection_)
				executableLinesSet.insert(lineInfo.lineNumber_);
			LOG_DEBUG << L"Executable lines for " << filePath << L": ";
			LOG_DEBUG << executableLinesSet;
			executableLineCache_.currentFilePath = filePath;
		}
		return executableLineCache_.executableLinesSet;
	}
}
