// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
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

#include <string>
#include <vector>

#include <memory>
#include <map>
#include <boost/optional.hpp>

#include "CoverageData.hpp"
#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class FileCoverage;

	class CPPCOVERAGE_DLL ExecutedAddressManager
	{
	public:
		ExecutedAddressManager();
		~ExecutedAddressManager();

		void SetCurrentModule(const std::wstring& moduleName);
		bool RegisterAddress(void* address, const std::wstring& filename, unsigned int line, unsigned char instruction);
		boost::optional<unsigned char> MarkAddressAsExecuted(void* address);

		CoverageData CreateCoverageData(const std::wstring& name, int exitCode) const;

	private:
		struct Module;
		struct File;
		struct File;
		struct Line;
		struct Instruction;

	private:
		ExecutedAddressManager(const ExecutedAddressManager&) = delete;
		ExecutedAddressManager& operator=(const ExecutedAddressManager&) = delete;

		Module& GetCurrentModule();
		void AddFileCoverageInfo(const File& fileData, FileCoverage& fileCoverage) const;
	
	private:
		std::vector<std::unique_ptr<Module>> modules_;
		std::map<void*, Instruction> addressLineMap_;
	};
}


