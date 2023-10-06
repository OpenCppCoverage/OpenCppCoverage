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

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL IDebugEventsHandler
	{
	public:
		enum class ExceptionType
		{
			BreakPoint,
			InvalidBreakPoint,
			NotHandled,
			Error,
			CppError
		};

		IDebugEventsHandler() = default;
		virtual ~IDebugEventsHandler() = default;

		virtual void OnCreateProcess(HANDLE hProces,const wchar_t* pszImageName,void* lpBaseOfImage);
		virtual void OnExitProcess(HANDLE hProcess);
		virtual void OnLoadDll(HANDLE hProcess, const wchar_t* pszImageName, void* lpBaseOfImage);
		virtual void OnUnloadDll(HANDLE hProcess, void* lpBaseOfImage);
		virtual ExceptionType OnException(HANDLE hProcess, HANDLE hThread, const EXCEPTION_DEBUG_INFO&);
		
	private:
		IDebugEventsHandler(const IDebugEventsHandler&) = delete;
		IDebugEventsHandler& operator=(const IDebugEventsHandler&) = delete;
	};	
}


