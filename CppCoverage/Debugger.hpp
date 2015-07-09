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

#include <boost/optional/optional.hpp>

#include <unordered_map>
#include <Windows.h>
#include "CppCoverageExport.hpp"


namespace CppCoverage
{
	class StartInfo;
	class IDebugEventsHandler;

	class CPPCOVERAGE_DLL Debugger
	{
	public:
		Debugger() = default;

		int Debug(const StartInfo&, IDebugEventsHandler&);

	private:
		Debugger(const Debugger&) = delete;
		Debugger& operator=(const Debugger&) = delete;
	
		void OnCreateProcess(
			const DEBUG_EVENT& debugEvent,
			IDebugEventsHandler& debugEventsHandler);

		int OnExitProcess(
			const DEBUG_EVENT& debugEvent,
			HANDLE hProcess,
			HANDLE hThread,
			IDebugEventsHandler& debugEventsHandler);

		void OnCreateThread(
			HANDLE hThread,
			DWORD dwThreadId);

		void OnExitThread(DWORD dwProcessId);

		HANDLE GetProcessHandle(DWORD dwProcessId) const;
		HANDLE GetThreadHandle(DWORD dwThreadId) const;

		struct ProcessStatus;

		ProcessStatus HandleDebugEvent(const DEBUG_EVENT&, IDebugEventsHandler&);

		ProcessStatus HandleNotCreationalEvent(
			const DEBUG_EVENT& debugEvent,
			IDebugEventsHandler& debugEventsHandler,
			HANDLE hProcess,
			HANDLE hThread,
			DWORD dwThreadId);

	private:
		std::unordered_map<DWORD, HANDLE> processHandles_;
		std::unordered_map<DWORD, HANDLE> threadHandles_;
		boost::optional<DWORD> rootProcessId_;
	};
}


