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

#include <Windows.h>
#include <map>
#include <set>
#include <boost/optional.hpp>

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

		void AddModule(const std::wstring& moduleName, void* dllBaseOfImage);
		void OnUnLoadModule(HANDLE hProcess, void* dllBaseOfImage);

		bool RegisterAddress(
			const Address&,
			const std::wstring& filename,
			unsigned int line,
			unsigned char instruction);

		boost::optional<unsigned char> MarkAddressAsExecuted(const Address&);

		CoverageData CreateCoverageData(const std::wstring& name, int exitCode) const;
		void OnExitProcess(HANDLE hProcess);

	private:
		struct Module;
		struct File;
		struct File;
		struct Line;
		struct LastModule
		{
			Module* module_;
			void* baseOfImage_;
		};

		ExecutedAddressManager(const ExecutedAddressManager&) = delete;
		ExecutedAddressManager& operator=(const ExecutedAddressManager&) = delete;

		Module& GetLastAddedModule();
		template <typename F>
		void RemoveAddressLineIf(F fct);

		std::map<std::wstring, Module> modules_;
		std::map<Address, Line> addressLineMap_;
		LastModule lastModule_;
	};
}
