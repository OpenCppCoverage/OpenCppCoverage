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

#include "MonitoredLineRegister.hpp"

#include <boost/uuid/uuid_generators.hpp>

#include "ICoverageFilterManager.hpp"
#include "Address.hpp"
#include "BreakPoint.hpp"
#include "ExecutedAddressManager.hpp"
#include "CppCoverageException.hpp"

#include "FileFilter/ModuleInfo.hpp"
#include "FileFilter/FileInfo.hpp"
#include "FileFilter/LineInfo.hpp"

namespace CppCoverage
{
	//----------------------------------------------------------------------------
	MonitoredLineRegister::MonitoredLineRegister(
	    std::shared_ptr<BreakPoint> breakPoint,
	    std::shared_ptr<ExecutedAddressManager> executedAddressManager,
	    std::shared_ptr<ICoverageFilterManager> coverageFilterManager)
	    : breakPoint_{breakPoint},
	      executedAddressManager_{executedAddressManager},
	      coverageFilterManager_{coverageFilterManager}
	{
	}

	//----------------------------------------------------------------------------
	void MonitoredLineRegister::RegisterLineToMonitor(
	    const boost::filesystem::path& modulePath,
	    HANDLE hProcess,
	    void* baseOfImage)
	{
		auto moduleUniqueId = boost::uuids::random_generator()();
		moduleInfo_ = std::make_unique<FileFilter::ModuleInfo>(
		    hProcess, moduleUniqueId, baseOfImage);

		DebugInformationEnumerator debugInformationEnumerator;
		debugInformationEnumerator.Enumerate(modulePath, *this);
	}

	//--------------------------------------------------------------------------
	bool MonitoredLineRegister::IsSourceFileSelected(
	    const boost::filesystem::path& path)
	{
		return coverageFilterManager_->IsSourceFileSelected(path.wstring());
	}

	//--------------------------------------------------------------------------
	void
	MonitoredLineRegister::OnSourceFile(const boost::filesystem::path& path,
	                                    const std::vector<Line>& lines)
	{
		std::vector<FileFilter::LineInfo> lineInfos;

		for (const auto& line : lines)
			lineInfos.emplace_back(
			    line.lineNumber_, line.virtualAddress_, 0);

		FileFilter::FileInfo fileInfo{path, std::move(lineInfos)};
		const auto& moduleInfo = GetModuleInfo();

		for (const auto& lineInfo : fileInfo.lineInfoColllection_)
		{
			auto lineNumber = lineInfo.lineNumber_;
			if (coverageFilterManager_->IsLineSelected(
			        moduleInfo, fileInfo, lineInfo))
			{
				auto addressValue =
				    lineInfo.virtualAddress_ +
				    reinterpret_cast<DWORD64>(moduleInfo.baseOfImage_);

				Address address{moduleInfo.hProcess_,
				                reinterpret_cast<void*>(addressValue)};

				auto oldInstruction = breakPoint_->SetBreakPointAt(address);

				if (!executedAddressManager_->RegisterAddress(
				        address, path.wstring(), lineNumber, oldInstruction))
				{
					breakPoint_->RemoveBreakPoint(address, oldInstruction);
				}
			}
		}
	}

	//--------------------------------------------------------------------------
	const FileFilter::ModuleInfo& MonitoredLineRegister::GetModuleInfo() const
	{
		if (!moduleInfo_)
			THROW("moduleInfo_ is null.");
		return *moduleInfo_;
	}
}
