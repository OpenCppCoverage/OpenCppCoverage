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

#include <Windows.h>
#include <boost/optional.hpp>

#include "CppCoverageExport.hpp"
#include "StartInfo.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL Process
	{
	public:
		Process(const StartInfo& startInfo);
		~Process();
		
		void Start(DWORD creationFlags);

	private:
		Process(const Process&) = delete;
		Process& operator=(const Process&) = delete;

	private:
		boost::optional<PROCESS_INFORMATION> processInformation_;
		const StartInfo startInfo_;
	};
}

