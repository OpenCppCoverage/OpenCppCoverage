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

#include "stdafx.h"
#include "TestTools.hpp"

#include <boost/filesystem.hpp>

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Debugger.hpp"
#include "CppCoverage/IDebugEventsHandler.hpp"

namespace bfs = boost::filesystem;
namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		struct DebugEventsHandler : public cov::IDebugEventsHandler
		{
			explicit DebugEventsHandler(TestTools::T_HandlesFct action)
			: action_(action)
			{
			}

			virtual void OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO& processInfo) override
			{
				action_(processInfo.hProcess, processInfo.hFile);
			}

		private:
			TestTools::T_HandlesFct action_;
		};

	}
		
	//-------------------------------------------------------------------------
	void TestTools::GetHandles(const boost::filesystem::path& path, T_HandlesFct action)
	{		
		cov::StartInfo startInfo{ path };
		cov::Debugger debugger;
		DebugEventsHandler debugEventsHandler{ action };

		debugger.Debug(startInfo, debugEventsHandler);
	}				
}

