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
#include "ReleaseCoverageFilter.hpp"

#include "Tools/Log.hpp"

#include "IRelocationsExtractor.hpp"
#include "RelocationsExtractor.hpp"
#include "ModuleInfo.hpp"
#include "FileInfo.hpp"
#include "LineInfo.hpp"

namespace FileFilter
{
	//-------------------------------------------------------------------------
	ReleaseCoverageFilter::ReleaseCoverageFilter()
		: ReleaseCoverageFilter{std::make_unique<RelocationsExtractor>()}
	{
	}

	//-------------------------------------------------------------------------
	ReleaseCoverageFilter::ReleaseCoverageFilter(
		std::unique_ptr<IRelocationsExtractor> relocationsExtractor)
		: relocationsExtractor_{ std::move(relocationsExtractor) }
	{
	}

	//-------------------------------------------------------------------------
	ReleaseCoverageFilter::~ReleaseCoverageFilter() = default;

	//-------------------------------------------------------------------------
	bool ReleaseCoverageFilter::IsLineSelected(
		const ModuleInfo& moduleInfo, 
		const FileInfo& fileInfo, 
		const LineInfo& lineInfo)
	{
		UpdateCachesIfExpired(moduleInfo, fileInfo);

		auto lineAddress = lineInfo.lineAddress_;
		if (lastSymbolAddresses_.count(lineAddress) == 0)
			return true;

		auto it = addressCountByLine_.find(lineInfo.lineNumber_);
		auto addressCount = (it == addressCountByLine_.end()) ? 0 : it->second;

		if (addressCount < 2)
			return true;

		if (relocations_.count(lineAddress) == 0)
			return true;

		LOG_DEBUG << "Optimized build support ignores line "
			<< lineInfo.lineNumber_
			<< " of " << fileInfo.filePath_.wstring();
		return false;
	}

	//-------------------------------------------------------------------------
	void ReleaseCoverageFilter::UpdateCachesIfExpired(
		const ModuleInfo& moduleInfo,
		const FileInfo& fileInfo)
	{
		auto hProcess = moduleInfo.hProcess_;
		auto moduleUniqueId = moduleInfo.uniqueId_;
		auto filePath = fileInfo.filePath_;

		auto updateRelocationsCache = moduleUniqueId != moduleUniqueId_;
		auto updateLineDataCaches = fileInfo.filePath_ != filePath_ || updateRelocationsCache;

		if (updateRelocationsCache)
		{
			relocations_ = relocationsExtractor_->Extract(
				hProcess, 
				reinterpret_cast<DWORD64>(moduleInfo.baseOfImage_), 
				moduleInfo.baseAddress_);
		}
		
		if (updateLineDataCaches)
			UpdateLineDataCaches(fileInfo.lineInfoColllection_);

		hProcess_ = hProcess;
		moduleUniqueId_ = moduleUniqueId;
		filePath_ = filePath;
	}

	//-------------------------------------------------------------------------
	void ReleaseCoverageFilter::UpdateLineDataCaches(const std::vector<LineInfo>& lineDatas)
	{
		std::unordered_map<ULONG, DWORD64> addressesBySymboleIndex;
		for (const auto& lineData: lineDatas)
		{
			auto lineAddress = lineData.lineAddress_;
			auto symbolIndex = lineData.symbolIndex_;
			auto lineNumber = lineData.lineNumber_;

			auto it = addressesBySymboleIndex.emplace(symbolIndex, 0).first;
			it->second = std::max(it->second, lineAddress);	
			++addressCountByLine_[lineNumber];
		}

		for (const auto& pair: addressesBySymboleIndex)
			lastSymbolAddresses_.insert(pair.second);
	}
}
