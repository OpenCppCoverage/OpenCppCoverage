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

		LOG_DEBUG << "ReleaseCoverageFilter: " 
			      << fileInfo.filePath_.wstring() << " " << lineInfo.lineNumber_;

		auto lineAddress = lineInfo.lineAddress_;
		if (lastSymbolAddresses_.count(lineAddress) == 0)
		{
			LOG_DEBUG << "\tNot the last symbol address.";
			return true;
		}

		auto it = linelAddressCounts_.find(lineAddress);
		auto lineAddressCount = (it == linelAddressCounts_.end()) ? 0 : it->second;

		if (lineAddressCount < 2)
		{
			LOG_DEBUG << "\tlineAddressCount < 2: " << lineAddressCount;
			return true;
		}

		if (relocations_.count(static_cast<DWORD_PTR>(lineAddress)))
		{
			LOG_DEBUG << "\tNot a relocation";
			return true;
		}

		LOG_DEBUG << "ReleaseCoverageFilter: Line removed";
		return false;
	}

	//-------------------------------------------------------------------------
	void ReleaseCoverageFilter::UpdateCachesIfExpired(
		const ModuleInfo& moduleInfo,
		const FileInfo& fileInfo)
	{
		auto hProcess = moduleInfo.hProcess_;
		auto hFileModule = moduleInfo.hFileModule_;
		auto filePath = fileInfo.filePath_;

		auto updateRelocationsCache = hProcess != hProcess_
								|| hFileModule != hFileModule_;

		auto updateLineDataCaches = fileInfo.filePath_ != filePath_ || updateRelocationsCache;

		if (updateRelocationsCache)
			relocations_ = relocationsExtractor_->Extract(hFileModule, moduleInfo.baseOfImage_);
		
		if (updateLineDataCaches)
			UpdateLineDataCaches(fileInfo.lineInfoColllection_);

		hProcess_ = hProcess;
		hFileModule_ = hFileModule;
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

			auto it = addressesBySymboleIndex.emplace(symbolIndex, 0).first;
			it->second = std::max(it->second, lineAddress);	
			++linelAddressCounts_[lineAddress];
		}

		for (const auto& pair: addressesBySymboleIndex)
			lastSymbolAddresses_.insert(pair.second);
	}
}
