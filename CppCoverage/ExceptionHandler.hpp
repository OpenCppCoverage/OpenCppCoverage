// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <Windows.h>
#include <iosfwd>
#include <unordered_map>
#include <map>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{	
	enum class ExceptionHandlerStatus
	{		
		BreakPoint,
		FirstChanceException,
		Error,
		CppError,
		SetThreadName
	};

	class CPPCOVERAGE_DLL ExceptionHandler
	{
	public:
		static const std::wstring UnhandledExceptionErrorMessage;
		static const std::wstring ExceptionCpp;
		static const std::wstring ExceptionSetThreadName;
		static const std::wstring ExceptionAccesViolation;
		static const std::wstring ExceptionUnknown;
		static const int ExceptionEmulationX86ErrorCode;
		static const int CppExceptionErrorCode;
		static const int SetThreadNameExceptionErrorCode;

		ExceptionHandler();

		ExceptionHandlerStatus HandleException(HANDLE hProcess, const EXCEPTION_DEBUG_INFO&, std::wostream&);
		void OnExitProcess(HANDLE hProcess);

	private:
		ExceptionHandler(const ExceptionHandler&) = delete;
		ExceptionHandler& operator=(const ExceptionHandler&) = delete;

		void InitExceptionCode();
		std::wstring GetExceptionStrFromCode(DWORD) const;

		std::unordered_map<DWORD, std::wstring> exceptionCode_;
		std::map<DWORD, std::vector<HANDLE>> breakPointExceptionCode_;
	};
}

