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

#define INITGUID
#include "dbgeng.h"

#include "Debugger.hpp"

#include "tools/Log.hpp"
#include "tools/ScopedAction.hpp"

#include "Process.hpp"
#include "CppCoverageException.hpp"
#include "IDebugEventsHandler.hpp"

#include "Tools/Tool.hpp"
#include "HandleInformation.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	namespace
	{
		void OnRip(const RIP_INFO& ripInfo)
		{
			LOG_ERROR << "Debugee process terminate unexpectedly:"
				<< "(type:" << ripInfo.dwType << ")"
				<< GetErrorMessage(ripInfo.dwError);
		}
	}	

	extern boost::optional<std::vector<wchar_t>> CreateCommandLine(const std::vector<std::wstring>& arguments);

	//-------------------------------------------------------------------------
	struct Debugger::ProcessStatus
	{
		ProcessStatus() = default;

		ProcessStatus(
			boost::optional<int> exitCode,
			boost::optional<DWORD> continueStatus)
			: exitCode_{ exitCode }
			, continueStatus_{ continueStatus }
		{
		}

		boost::optional<int> exitCode_;
		boost::optional<DWORD> continueStatus_;
	};

	//-------------------------------------------------------------------------
	Debugger::Debugger(
		bool coverChildren,
		bool continueAfterCppException,
        bool stopOnAssert)
		: coverChildren_{ coverChildren }
		, continueAfterCppException_{ continueAfterCppException }
        , stopOnAssert_{ stopOnAssert }
	{
	}

	//-------------------------------------------------------------------------
	int Debugger::Debug(
		const StartInfo& startInfo,
		IDebugEventsHandler& debugEventsHandler)
	{

		// Try to load Microsoft Debug Engine

		HMODULE hDll = LoadLibrary(L"dbgeng.dll");
		if (hDll == NULL) {

			LOG_DEBUG << "No dbgeng.dll found, using debug API directly";

			Process process(startInfo);
			process.Start((coverChildren_) ? DEBUG_PROCESS : DEBUG_ONLY_THIS_PROCESS);

			DEBUG_EVENT debugEvent;
			boost::optional<int> exitCode;

			processHandles_.clear();
			threadHandles_.clear();
			rootProcessId_ = boost::none;

			while (!exitCode || !processHandles_.empty())
			{
				if (!WaitForDebugEvent(&debugEvent, INFINITE))
					THROW_LAST_ERROR(L"Error WaitForDebugEvent:", GetLastError());

				ProcessStatus processStatus = HandleDebugEvent(debugEvent, debugEventsHandler);

				// Get the exit code of the root process
				// Set once as we do not want EXCEPTION_BREAKPOINT to be override
				if (processStatus.exitCode_ && rootProcessId_ == debugEvent.dwProcessId && !exitCode)
					exitCode = processStatus.exitCode_;

				auto continueStatus = boost::get_optional_value_or(processStatus.continueStatus_, DBG_CONTINUE);

				if (!ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, continueStatus))
					THROW_LAST_ERROR("Error in ContinueDebugEvent:", GetLastError());
			}
			return *exitCode;
		}
		else {
			
			// Save the event handler callback pointer
			debugEventsHandler_ = &debugEventsHandler;

			// Locate the DebugCreate function in dbgeng.dll
			PFN_DebugCreate pfnDebugCreate = (PFN_DebugCreate)GetProcAddress(hDll, "DebugCreate");
			if (pfnDebugCreate == NULL) {
				THROW("Failed to locate DebugCreate in dbgeng.dll");
			}

			// Create a debug client instance
			HRESULT hr = pfnDebugCreate(IID_IDebugClient5, (PVOID*)&pDebug_);
			if (S_OK != hr) {
				THROW("Failed to create debugger in dbgeng.dll");
			}

			// Locate the DebugControl interface of the debug client
			hr = pDebug_->QueryInterface(IID_IDebugControl, (void**)&pDebugControl_);
			if (S_OK != hr) {
				THROW("Failed to narrow to dbgeng to IID_IDebugControl");
			}

			// Locate the DebugSystemObject interface of the debug client
			hr = pDebug_->QueryInterface(IID_IDebugSystemObjects, (void**)&pDebugSystemObjects_);
			if (S_OK != hr) {
				THROW("Failed to narrow to dbgeng to IID_IDebugSystemObjects");
			}

			// Install callbacks pointing to this object. 
			pDebug_->SetEventCallbacksWide(this);
			pDebug_->SetOutputCallbacksWide(this);

			// Make sure we get an initial breakpoint
			pDebugControl_->AddEngineOptions(DEBUG_ENGOPT_INITIAL_BREAK);

			// Build the command line for the process to debug
			auto optionalCommandLine = CreateCommandLine(startInfo.GetArguments());

			// Run the process under the debugger
			hr = pDebug_->CreateProcessAndAttachWide(0, &(*optionalCommandLine)[0], (coverChildren_) ? DEBUG_PROCESS : DEBUG_ONLY_THIS_PROCESS, 0, 0);
			if (S_OK != hr) {
				THROW("Failed to run process");
			}

			// Process debugger events until debug client gives an error
			for(;;)
			{
				hr = pDebugControl_->WaitForEvent(0, 1000);
				if (FAILED(hr)) {
					break;
				}
			}

			// Disconnect our events
			pDebug_->SetEventCallbacksWide(NULL);
			pDebug_->SetOutputCallbacksWide(NULL);

			// Release debug client
			pDebugSystemObjects_->Release();
			pDebugControl_->Release();
			pDebug_->Release();

			// Unload DLL
			FreeLibrary(hDll);

			// FIX: Capture and save the exit code
			return 0;	//  *exitCode;
		}
	}
	
	//-------------------------------------------------------------------------
	Debugger::ProcessStatus Debugger::HandleDebugEvent(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler)
	{
		auto dwProcessId = debugEvent.dwProcessId;
		auto dwThreadId = debugEvent.dwThreadId;

		switch (debugEvent.dwDebugEventCode)
		{
			case CREATE_PROCESS_DEBUG_EVENT: OnCreateProcess(debugEvent, debugEventsHandler); break;
			case CREATE_THREAD_DEBUG_EVENT: OnCreateThread(debugEvent.u.CreateThread.hThread, dwThreadId); break;
			default:
			{
				auto hProcess = GetProcessHandle(dwProcessId);
				auto hThread = GetThreadHandle(dwThreadId);
				return HandleNotCreationalEvent(debugEvent, debugEventsHandler, hProcess, hThread, dwThreadId);
			}
		}

		return{};
	}

	//-------------------------------------------------------------------------
	Debugger::ProcessStatus
		Debugger::HandleNotCreationalEvent(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler,
		HANDLE hProcess,
		HANDLE hThread,
		DWORD dwThreadId)
	{
		switch (debugEvent.dwDebugEventCode)
		{
			case EXIT_PROCESS_DEBUG_EVENT:
			{
				auto exitCode = OnExitProcess(debugEvent, hProcess, hThread, debugEventsHandler);
				return ProcessStatus{exitCode, boost::none};
			}
			case EXIT_THREAD_DEBUG_EVENT: OnExitThread(dwThreadId); break;
			case LOAD_DLL_DEBUG_EVENT:
			{
				const auto& loadDll = debugEvent.u.LoadDll;
				Tools::ScopedAction scopedAction{ [&loadDll]{ CloseHandle(loadDll.hFile); } };
				HandleInformation handleInformation;

				std::wstring filename = handleInformation.ComputeFilename(loadDll.hFile);

				debugEventsHandler.OnLoadDll(hProcess, filename.c_str(), loadDll.lpBaseOfDll);
				break;
			}
			case UNLOAD_DLL_DEBUG_EVENT:
			{
				debugEventsHandler.OnUnloadDll(hProcess, debugEvent.u.UnloadDll.lpBaseOfDll);
				break;
			}
			case EXCEPTION_DEBUG_EVENT: return OnException(debugEvent, debugEventsHandler, hProcess, hThread);
			case RIP_EVENT: OnRip(debugEvent.u.RipInfo); break;
			default: LOG_DEBUG << "Debug event:" << debugEvent.dwDebugEventCode; break;
		}

		return ProcessStatus{};
	}
	
	//-------------------------------------------------------------------------
	Debugger::ProcessStatus
		Debugger::OnException(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler,
		HANDLE hProcess,
		HANDLE hThread) const
	{
		const auto& exception = debugEvent.u.Exception;
		auto exceptionType = debugEventsHandler.OnException(hProcess, hThread, exception);

		switch (exceptionType)
		{
			case IDebugEventsHandler::ExceptionType::BreakPoint:
			{
				return ProcessStatus{ boost::none, DBG_CONTINUE };
			}
			case IDebugEventsHandler::ExceptionType::InvalidBreakPoint:
			{
				LOG_WARNING << Tools::GetSeparatorLine();
				LOG_WARNING << "It seems there is an assertion failure or you call DebugBreak() in your program.";
				LOG_WARNING << Tools::GetSeparatorLine();

                if (stopOnAssert_)
                {
                  LOG_WARNING << "Stop on assertion.";
                  return ProcessStatus{ boost::none, DBG_EXCEPTION_NOT_HANDLED };
                }
                else
                {
                  return ProcessStatus(EXCEPTION_BREAKPOINT, DBG_CONTINUE);
                }
			}
			case IDebugEventsHandler::ExceptionType::NotHandled:
			{
				return ProcessStatus{ boost::none, DBG_EXCEPTION_NOT_HANDLED };
			}
			case IDebugEventsHandler::ExceptionType::Error:
			{
				return ProcessStatus{ boost::none, DBG_EXCEPTION_NOT_HANDLED };
			}
			case IDebugEventsHandler::ExceptionType::CppError:
			{
				if (continueAfterCppException_)
				{
					const auto& exceptionRecord = exception.ExceptionRecord;
					LOG_WARNING << "Continue after a C++ exception.";
					return ProcessStatus{ static_cast<int>(exceptionRecord.ExceptionCode), DBG_CONTINUE };
				}
				return ProcessStatus{ boost::none, DBG_EXCEPTION_NOT_HANDLED };
			}
		}
		THROW("Invalid exception Type.");
	}

	//-------------------------------------------------------------------------
	void Debugger::OnCreateProcess(
		const DEBUG_EVENT& debugEvent,
		IDebugEventsHandler& debugEventsHandler)
	{		
		const auto& processInfo = debugEvent.u.CreateProcessInfo;
		Tools::ScopedAction scopedAction{ [&processInfo]{ CloseHandle(processInfo.hFile); } };

		LOG_DEBUG << "Create Process:" << debugEvent.dwProcessId;

		if (!rootProcessId_ && processHandles_.empty())
			rootProcessId_ = debugEvent.dwProcessId;

		if (!processHandles_.emplace(debugEvent.dwProcessId, processInfo.hProcess).second)
			THROW("Process id already exist");
				
		HandleInformation handleInformation;

		std::wstring filename = handleInformation.ComputeFilename(processInfo.hFile);

		debugEventsHandler.OnCreateProcess(processInfo.hProcess,filename.c_str(),processInfo.lpBaseOfImage);

		OnCreateThread(processInfo.hThread, debugEvent.dwThreadId);
	}

	//-------------------------------------------------------------------------
	int Debugger::OnExitProcess(
		const DEBUG_EVENT& debugEvent,
		HANDLE hProcess,
		HANDLE hThread,
		IDebugEventsHandler& debugEventsHandler)
	{
		OnExitThread(debugEvent.dwThreadId);
		auto processId = debugEvent.dwProcessId;

		LOG_DEBUG << "Exit Process:" << processId;

		auto exitProcess = debugEvent.u.ExitProcess;
		debugEventsHandler.OnExitProcess(hProcess);

		if (processHandles_.erase(processId) != 1)
			THROW("Cannot find exited process.");

		return exitProcess.dwExitCode;
	}

	//-------------------------------------------------------------------------
	void Debugger::OnCreateThread(
		HANDLE hThread,
		DWORD dwThreadId)
	{
		LOG_DEBUG << "Create Thread:" << dwThreadId;

		if (!threadHandles_.emplace(dwThreadId, hThread).second)
			THROW("Thread id already exist");
	}
	
	//-------------------------------------------------------------------------
	void Debugger::OnExitThread(DWORD dwThreadId)
	{	
		LOG_DEBUG << "Exit thread:" << dwThreadId;

		if (threadHandles_.erase(dwThreadId) != 1)
			THROW("Cannot find exited thread.");
	}

	//-------------------------------------------------------------------------
	HANDLE Debugger::GetProcessHandle(DWORD dwProcessId) const
	{
		return processHandles_.at(dwProcessId);
	}

	//-------------------------------------------------------------------------
	HANDLE Debugger::GetThreadHandle(DWORD dwThreadId) const
	{
		return threadHandles_.at(dwThreadId);
	}

	//-------------------------------------------------------------------------
	size_t Debugger::GetRunningProcesses() const
	{
		return processHandles_.size();
	}

	//-------------------------------------------------------------------------
	size_t Debugger::GetRunningThreads() const
	{
		return threadHandles_.size();
	}

	HRESULT Debugger::QueryInterface(REFIID InterfaceId, PVOID* Interface) {
		HRESULT     hr;

		if (InlineIsEqualGUID(InterfaceId, IID_IDebugEventCallbacksWide)) {
			*Interface = (IDebugEventCallbacksWide*)this;
			return S_OK;
		}
		if (InlineIsEqualGUID(InterfaceId, IID_IDebugOutputCallbacksWide)) {
			*Interface = (IDebugOutputCallbacksWide*)this;
			return S_OK;
		}

		// We probably does not support this interface. Log it to be able to see that interface is requested
		LPOLESTR    str;

		hr = StringFromIID(InterfaceId, &str);
		if (FAILED(hr)) {
			return E_NOINTERFACE;
		}

		LOG_DEBUG << "QueryInterface for unsupported interface" << str;

		//free the string
		LPMALLOC pMalloc;
		CoGetMalloc(1, &pMalloc);
		pMalloc->Free(str);
		pMalloc->Release();

		return E_NOINTERFACE;
	}

	ULONG Debugger::AddRef() {
		// Faked AddRef since this instance is not a COM instance
		return 1;
	}

	ULONG Debugger::Release() {
		// Faked Release since this instance is not a COM instance
		return 1;
	}

	// The engine calls GetInterestMask once when
	// the event callbacks are set for a client.
	HRESULT Debugger::GetInterestMask(PULONG Mask) {
		// Request all events
		*Mask = 0xffffffff;
		return S_OK;
	}

	// A breakpoint event is generated when
	// a breakpoint exception is received and
	// it can be mapped to an existing breakpoint.
	// The callback method is given a reference
	// to the breakpoint and should release it when
	// it is done with it.
	HRESULT Debugger::Breakpoint(PDEBUG_BREAKPOINT2 Bp) {
		LOG_INFO << "Breakpoint";
		return DEBUG_STATUS_NO_CHANGE;
	}

	// Exceptions include breaks which cannot
	// be mapped to an existing breakpoint
	// instance.
	HRESULT Debugger::Exception(PEXCEPTION_RECORD64 Exception, ULONG FirstChance) {
		HRESULT					hr;
		HANDLE					hProcess;
		HANDLE					hThread;
		EXCEPTION_DEBUG_INFO	di;

		// winnt.h declares both EXCEPTION_RECORD and EXCEPTION_RECORD64 with very similar content.
		// Map the content
		di.ExceptionRecord.ExceptionAddress = (PVOID)Exception->ExceptionAddress;
		di.ExceptionRecord.ExceptionCode = Exception->ExceptionCode;
		di.ExceptionRecord.ExceptionFlags = Exception->ExceptionFlags;
		di.ExceptionRecord.NumberParameters = Exception->NumberParameters;
		memcpy(di.ExceptionRecord.ExceptionInformation, Exception->ExceptionInformation, sizeof(di.ExceptionRecord.ExceptionInformation));
		di.ExceptionRecord.ExceptionRecord = NULL;
		di.dwFirstChance = FirstChance;

		hr = pDebugSystemObjects_->GetCurrentProcessHandle((PULONG64)&hProcess);
		if (FAILED(hr)) {
			THROW("GetCurrentProcessHandle failed");
		}

		hr = pDebugSystemObjects_->GetCurrentThreadHandle((PULONG64)&hThread);
		if (FAILED(hr)) {
			THROW("GetCurrentThreadHandle failed");
		}

		auto exceptionType = debugEventsHandler_->OnException(hProcess, hThread,di);

		switch (exceptionType)
		{
		case IDebugEventsHandler::ExceptionType::BreakPoint:
		{
			return DEBUG_STATUS_NO_CHANGE;
		}
		case IDebugEventsHandler::ExceptionType::InvalidBreakPoint:
		{
			LOG_WARNING << Tools::GetSeparatorLine();
			LOG_WARNING << "It seems there is an assertion failure or you call DebugBreak() in your program.";
			LOG_WARNING << Tools::GetSeparatorLine();

			if (stopOnAssert_)
			{
				LOG_WARNING << "Stop on assertion.";
				return DEBUG_STATUS_BREAK;
			}
			else
			{
				return DEBUG_STATUS_NO_CHANGE;
			}
		}
		case IDebugEventsHandler::ExceptionType::NotHandled:
		{
			return DEBUG_STATUS_BREAK;
		}
		case IDebugEventsHandler::ExceptionType::Error:
		{
			return DEBUG_STATUS_BREAK;
		}
		case IDebugEventsHandler::ExceptionType::CppError:
		{
			if (continueAfterCppException_)
			{
				LOG_WARNING << "Continue after a C++ exception.";
				return DEBUG_STATUS_NO_CHANGE;
			}
			return DEBUG_STATUS_BREAK;
		}
		}

		return DEBUG_STATUS_NO_CHANGE;
	}

	// Any of these values can be zero if they
	// cannot be provided by the engine.
	// Currently the kernel does not return thread
	// or process change events.
	HRESULT Debugger::CreateThread(ULONG64 Handle, ULONG64 DataOffset, ULONG64 StartOffset) {
		HRESULT		hr;
		DWORD		dwThreadId;

		hr = pDebugSystemObjects_->GetCurrentThreadSystemId(&dwThreadId);
		if (FAILED(hr)) {
			THROW("GetCurrentThreadSystemId failed");
		}

		OnCreateThread((HANDLE)Handle, dwThreadId);

		return DEBUG_STATUS_NO_CHANGE;
	}

	HRESULT Debugger::ExitThread(ULONG ExitCode) {
		HRESULT		hr;
		DWORD		dwThreadId;

		hr = pDebugSystemObjects_->GetCurrentThreadSystemId(&dwThreadId);
		if (FAILED(hr)) {
			THROW("GetCurrentThreadSystemId failed");
		}

		OnExitThread(dwThreadId);

		return DEBUG_STATUS_NO_CHANGE;
	}

	// Any of these values can be zero if they
	// cannot be provided by the engine.
	HRESULT Debugger::CreateProcessW(ULONG64 ImageFileHandle, ULONG64 Handle, ULONG64 BaseOffset, ULONG ModuleSize, PCWSTR ModuleName, PCWSTR ImageName, ULONG CheckSum, ULONG TimeDateStamp, ULONG64 InitialThreadHandle, ULONG64 ThreadDataOffset, ULONG64 StartOffset) {
		HRESULT		hr;
		DWORD		dwThreadId;
		DWORD		dwProcessId;
		HANDLE		hProcess;

		hr = pDebugSystemObjects_->GetCurrentThreadSystemId(&dwThreadId);
		if (FAILED(hr)) {
			THROW("GetCurrentThreadSystemId failed");
		}

		hr = pDebugSystemObjects_->GetCurrentProcessSystemId(&dwProcessId);
		if (FAILED(hr)) {
			THROW("GetCurrentProcessSystemId failed");
		}

		hr = pDebugSystemObjects_->GetCurrentProcessHandle((PULONG64)&hProcess);
		if (FAILED(hr)) {
			THROW("GetCurrentProcessHandle failed");
		}

		// The the full EXE filename
		WCHAR   FullImageName[1024];
		ULONG   cbFullImageName;

		hr = pDebug_->GetRunningProcessDescriptionWide(0, dwProcessId, 0, FullImageName, sizeof(FullImageName), &cbFullImageName, NULL, 0, NULL);
		if (FAILED(hr)) {
			THROW("GetRunningProcessDescriptionWide failed");
		}

		LOG_DEBUG << "Create Process " << dwProcessId << " " << FullImageName;

		if (!rootProcessId_ && processHandles_.empty())
			rootProcessId_ = dwProcessId;

		if (!processHandles_.emplace(dwProcessId, hProcess).second)
			THROW("Process id already exist");

		debugEventsHandler_->OnCreateProcess((HANDLE)Handle, FullImageName, (void*)BaseOffset);

		OnCreateThread((HANDLE)InitialThreadHandle, dwThreadId);

		return DEBUG_STATUS_NO_CHANGE;
	}

	HRESULT Debugger::ExitProcess(ULONG ExitCode) {
		HRESULT		hr;
		DWORD		dwThreadId;
		DWORD		dwProcessId;
		HANDLE		hProcess;

		hr = pDebugSystemObjects_->GetCurrentThreadSystemId(&dwThreadId);
		if (FAILED(hr)) {
			THROW("GetCurrentThreadSystemId failed");
		}

		OnExitThread(dwThreadId);

		hr = pDebugSystemObjects_->GetCurrentProcessSystemId(&dwProcessId);
		if (FAILED(hr)) {
			THROW("GetCurrentProcessSystemId failed");
		}

		hr = pDebugSystemObjects_->GetCurrentProcessHandle((PULONG64)&hProcess);
		if (FAILED(hr)) {
			THROW("GetCurrentProcessHandle failed");
		}

		LOG_DEBUG << "Exit Process:" << dwProcessId;

		debugEventsHandler_->OnExitProcess(hProcess);

		if (processHandles_.erase(dwProcessId) != 1)
			THROW("Cannot find exited process.");

		return DEBUG_STATUS_NO_CHANGE;
	}

	// Any of these values may be zero.
	HRESULT Debugger::LoadModule(ULONG64 ImageFileHandle, ULONG64 BaseOffset, ULONG ModuleSize, PCWSTR ModuleName, PCWSTR ImageName, ULONG CheckSum, ULONG TimeDateStamp) {
		HRESULT		hr;
		HANDLE		hProcess;

		hr = pDebugSystemObjects_->GetCurrentProcessHandle((PULONG64)&hProcess);
		if (FAILED(hr)) {
			THROW("GetCurrentProcessHandle failed");
		}

		LOG_DEBUG << "LoadModule " << ImageName << " " << BaseOffset;

		debugEventsHandler_->OnLoadDll(hProcess, ImageName, (void*)BaseOffset);

		return DEBUG_STATUS_NO_CHANGE;
	}

	HRESULT Debugger::UnloadModule(PCWSTR ImageBaseName, ULONG64 BaseOffset) {
		HRESULT		hr;
		HANDLE		hProcess;

		hr = pDebugSystemObjects_->GetCurrentProcessHandle((PULONG64)&hProcess);
		if (FAILED(hr)) {
			THROW("GetCurrentProcessHandle failed");
		}

		LOG_DEBUG << "UnloadModule " << BaseOffset;

		debugEventsHandler_->OnUnloadDll(hProcess, (void*)BaseOffset);

		return DEBUG_STATUS_NO_CHANGE;
	}

	HRESULT Debugger::SystemError(ULONG Error, ULONG Level) {
		return DEBUG_STATUS_NO_CHANGE;
	}

	// Session status is synchronous like the other
	// wait callbacks but it is called as the state
	// of the session is changing rather than at
	// specific events so its return value does not
	// influence waiting.  Implementations should just
	// return DEBUG_STATUS_NO_CHANGE.
	// Also, because some of the status
	// notifications are very early or very
	// late in the session lifetime there may not be
	// current processes or threads when the notification
	// is generated.
	HRESULT Debugger::SessionStatus(ULONG Status) {
		return DEBUG_STATUS_NO_CHANGE;
	}

	// The following callbacks are informational
	// callbacks notifying the provider about
	// changes in debug state.  The return value
	// of these callbacks is ignored.  Implementations
	// can not call back into the engine.

	// Debuggee state, such as registers or data spaces,
	// has changed.
	HRESULT Debugger::ChangeDebuggeeState(ULONG Flags, ULONG64 Argument) {
		return S_OK;
	}

	// Engine state has changed.
	HRESULT Debugger::ChangeEngineState(ULONG Flags, ULONG64 Argument) {
		return S_OK;
	}

	// Symbol state has changed.
	HRESULT Debugger::ChangeSymbolState(ULONG Flags, ULONG64 Argument) {
		return S_OK;
	}

	HRESULT Debugger::Output(ULONG Mask, PCWSTR Text) {
		return 0;
	}

}
