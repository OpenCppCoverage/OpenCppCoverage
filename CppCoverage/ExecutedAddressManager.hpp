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
#include <unordered_map>

#include <Windows.h>
#include <memory>
#include <map>
#include <boost/optional.hpp>
#include <boost/container/small_vector.hpp>

#include "CoverageData.hpp"
#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class FileCoverage;
	class Address;

	class CPPCOVERAGE_DLL ExecutedAddressManager
	{
	public:
		ExecutedAddressManager();
		~ExecutedAddressManager();

		void AddModule(HANDLE hProcess, const std::wstring& moduleName);
		bool RegisterAddress(
			const Address&,
			const std::wstring& filename,
			unsigned int line,
			unsigned char instruction);

		boost::optional<unsigned char> MarkAddressAsExecuted(const Address&);

		CoverageData CreateCoverageData(const std::wstring& name, int exitCode);
		void OnExitProcess(HANDLE hProcess);

	private:
		struct Module;
		struct File;
		struct File;

	private:
		ExecutedAddressManager(const ExecutedAddressManager&) = delete;
		ExecutedAddressManager& operator=(const ExecutedAddressManager&) = delete;

		using T_Modules = std::vector<std::unique_ptr<Module>>;

		Module& GetLastAddedModule(HANDLE hProcess);
		void AddModulesToCoverageData(HANDLE hProcess);
		CoverageData CreateCoverageData(const T_Modules&) const;

	private:
		std::unordered_map<HANDLE, T_Modules> modulesByHandle_;

		CoverageData coverageData_;

		struct Line
		{
			explicit Line(unsigned char instructionToRestore);

			unsigned char instructionToRestore_;
			boost::container::small_vector<bool*, 1> hasBeenExecutedCollection_;
		};

		std::map<Address, Line> addressLineMap_;
	};
}
