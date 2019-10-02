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

		auto lineAddress = lineInfo.virtualAddress_;
		if (mModuleData_->fileData_->lastSymbolAddresses_.count(lineAddress) == 0)
			return true;

		auto& addressCountByLine = mModuleData_->fileData_->addressCountByLine_;
		auto it = addressCountByLine.find(lineInfo.lineNumber_);
		auto addressCount = (it == addressCountByLine.end()) ? 0 : it->second;

		if (addressCount < 2)
			return true;

		if (mModuleData_->relocations_.count(lineAddress) == 0)
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
		auto modulePath = moduleInfo.path_;
		auto filePath = fileInfo.filePath_;

		if (!mModuleData_ || mModuleData_->path_ != modulePath)
		{
			mModuleData_ = std::make_unique<ModuleData>();
			mModuleData_->path_ = modulePath;
			mModuleData_->relocations_ = relocationsExtractor_->Extract(
				moduleInfo.hProcess_,
				reinterpret_cast<DWORD64>(moduleInfo.baseOfImage_));
		}
		
		if (!mModuleData_->fileData_ || mModuleData_->fileData_->path_ != filePath)
			mModuleData_->fileData_ = UpdateLineDataCaches(filePath, fileInfo.lineInfoColllection_);
	}

	//-------------------------------------------------------------------------
	std::unique_ptr<ReleaseCoverageFilter::FileData>
	ReleaseCoverageFilter::UpdateLineDataCaches(
	    const std::filesystem::path& filePath, const std::vector<LineInfo>& lineDatas)
	{
		auto fileData = std::make_unique<FileData>();
		fileData->path_ = filePath;

		std::unordered_map<ULONG, DWORD64> addressesBySymboleIndex;
		for (const auto& lineData: lineDatas)
		{
			auto lineAddress = lineData.virtualAddress_;
			auto symbolIndex = lineData.symbolIndex_;
			auto lineNumber = lineData.lineNumber_;

			auto it = addressesBySymboleIndex.emplace(symbolIndex, 0).first;
			it->second = std::max(it->second, lineAddress);	
			++fileData->addressCountByLine_[lineNumber];
		}

		for (const auto& pair: addressesBySymboleIndex)
			fileData->lastSymbolAddresses_.insert(pair.second);
		return fileData;
	}
}
