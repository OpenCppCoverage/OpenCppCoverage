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

#pragma once

#include "DebugInformationEnumerator.hpp"
#include <memory>
#include <unordered_map>
#include <filesystem>

namespace FileFilter
{
	class LineInfo;
	class ModuleInfo;
}

namespace CppCoverage
{
	class ICoverageFilterManager;
	class BreakPoint;
	class ExecutedAddressManager;
	class FilterAssistant;

	class MonitoredLineRegister : private IDebugInformationHandler
	{
	  public:
		MonitoredLineRegister(std::shared_ptr<BreakPoint>,
		                      std::shared_ptr<ExecutedAddressManager>,
		                      std::shared_ptr<ICoverageFilterManager>,
		                      std::unique_ptr<DebugInformationEnumerator>,
		                      std::shared_ptr<FilterAssistant>);
		~MonitoredLineRegister();

		bool RegisterLineToMonitor(const std::filesystem::path& modulePath,
		                           HANDLE hProcess,
		                           void* baseOfImage);

	  private:
		bool IsSourceFileSelected(const std::filesystem::path&) override;
		void OnSourceFile(const std::filesystem::path&,
		                  const std::vector<Line>&) override;

		using LineNumberByAddress =
		    std::unordered_map<DWORD64, std::vector<int>>;
		void SetBreakPoint(const std::filesystem::path&,
		                   HANDLE hProcess,
		                   std::vector<DWORD64>&&,
		                   const LineNumberByAddress&);

		const FileFilter::ModuleInfo& GetModuleInfo() const;

		std::unique_ptr<FileFilter::ModuleInfo> moduleInfo_;
		const std::shared_ptr<BreakPoint> breakPoint_;
		const std::shared_ptr<ExecutedAddressManager> executedAddressManager_;
		const std::shared_ptr<ICoverageFilterManager> coverageFilterManager_;
		const std::unique_ptr<DebugInformationEnumerator>
		    debugInformationEnumerator_;
		const std::shared_ptr<FilterAssistant> filterAssistant_;
	};
}
