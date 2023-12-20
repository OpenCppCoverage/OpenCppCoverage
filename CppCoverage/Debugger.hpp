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

#include "dbgeng.h"

namespace CppCoverage
{
	class StartInfo;
	class IDebugEventsHandler;

	class CPPCOVERAGE_DLL Debugger : public IDebugEventCallbacksWide, public IDebugOutputCallbacksWide
	{
	public:
		Debugger(
			bool coverChildren,
			bool continueAfterCppException,
			bool stopOnAssert);

		int Debug(const StartInfo&, IDebugEventsHandler&);
		size_t GetRunningProcesses() const;
		size_t GetRunningThreads() const;

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

		ProcessStatus OnException(const DEBUG_EVENT&, IDebugEventsHandler&, HANDLE hProcess, HANDLE hThread) const;

	private:
		std::unordered_map<DWORD, HANDLE> processHandles_;
		std::unordered_map<DWORD, HANDLE> threadHandles_;
		boost::optional<DWORD> rootProcessId_;
		bool coverChildren_;
		bool continueAfterCppException_;
		bool stopOnAssert_;

	public:
		// IUnknown
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID InterfaceId, PVOID* Interface);
		ULONG STDMETHODCALLTYPE AddRef();
		ULONG STDMETHODCALLTYPE Release();
		// IDebugEventCallbacksWide
		HRESULT STDMETHODCALLTYPE GetInterestMask(PULONG Mask);
		HRESULT STDMETHODCALLTYPE Breakpoint(PDEBUG_BREAKPOINT2 Bp);
		HRESULT STDMETHODCALLTYPE Exception(PEXCEPTION_RECORD64 Exception, ULONG FirstChance);
		HRESULT STDMETHODCALLTYPE CreateThread(ULONG64 Handle, ULONG64 DataOffset, ULONG64 StartOffset);
		HRESULT STDMETHODCALLTYPE ExitThread(ULONG ExitCode);
		HRESULT STDMETHODCALLTYPE CreateProcessW(ULONG64 ImageFileHandle, ULONG64 Handle, ULONG64 BaseOffset, ULONG ModuleSize, PCWSTR ModuleName, PCWSTR ImageName, ULONG CheckSum, ULONG TimeDateStamp, ULONG64 InitialThreadHandle, ULONG64 ThreadDataOffset, ULONG64 StartOffset);
		HRESULT STDMETHODCALLTYPE ExitProcess(ULONG ExitCode);
		HRESULT STDMETHODCALLTYPE LoadModule(ULONG64 ImageFileHandle, ULONG64 BaseOffset, ULONG ModuleSize, PCWSTR ModuleName, PCWSTR ImageName, ULONG CheckSum, ULONG TimeDateStamp);
		HRESULT STDMETHODCALLTYPE UnloadModule(PCWSTR ImageBaseName, ULONG64 BaseOffset);
		HRESULT STDMETHODCALLTYPE SystemError(ULONG Error, ULONG Level);
		HRESULT STDMETHODCALLTYPE SessionStatus(ULONG Status);
		HRESULT STDMETHODCALLTYPE ChangeDebuggeeState(ULONG Flags, ULONG64 Argument);
		HRESULT STDMETHODCALLTYPE ChangeEngineState(ULONG Flags, ULONG64 Argument);
		HRESULT STDMETHODCALLTYPE ChangeSymbolState(ULONG Flags, ULONG64 Argument);
		// IDebugOutputCallbacksWide
		HRESULT STDMETHODCALLTYPE Output(ULONG Mask, PCWSTR Text);
	private:
		IDebugEventsHandler* debugEventsHandler_;
		IDebugClient6* pDebug_;
		IDebugControl* pDebugControl_;
		IDebugSystemObjects* pDebugSystemObjects_;

		enum eDebugState {
			CONTINUE,
			DONE,
			TERMINATE
		};
		eDebugState		debugState_;
		int				debugExitCode_;
	};
}

typedef HRESULT(__stdcall* PFN_DebugCreate)(REFIID InterfaceId, PVOID* Interface);



