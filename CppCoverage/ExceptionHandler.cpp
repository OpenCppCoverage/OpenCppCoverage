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
#include "ExceptionHandler.hpp"

#include "Tools/ScopedAction.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	const std::wstring ExceptionHandler::UnhandledExceptionErrorMessage = L"Unhandled exception code: ";
	const std::wstring ExceptionHandler::ExceptionCpp = L"Exception C++";
	const std::wstring ExceptionHandler::ExceptionAccesViolation = L"EXCEPTION_ACCESS_VIOLATION";
	const std::wstring ExceptionHandler::ExceptionUnknown = L"Unknown";
	const int ExceptionHandler::ExceptionEmulationX86ErroCode = 0x4000001f;

	//-------------------------------------------------------------------------
	ExceptionHandler::ExceptionHandler()
	{
		breakPointExceptionCode_.emplace(EXCEPTION_BREAKPOINT, true);
		breakPointExceptionCode_.emplace(ExceptionEmulationX86ErroCode, true);
		InitExceptionCode();
	}

	//-------------------------------------------------------------------------
	void ExceptionHandler::InitExceptionCode()
	{
		exceptionCode_.emplace(EXCEPTION_ACCESS_VIOLATION, L"EXCEPTION_ACCESS_VIOLATION");
		exceptionCode_.emplace(EXCEPTION_ARRAY_BOUNDS_EXCEEDED, L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
		exceptionCode_.emplace(EXCEPTION_BREAKPOINT, L"EXCEPTION_BREAKPOINT");
		exceptionCode_.emplace(EXCEPTION_DATATYPE_MISALIGNMENT, L"EXCEPTION_DATATYPE_MISALIGNMENT");
		exceptionCode_.emplace(EXCEPTION_FLT_DENORMAL_OPERAND, L"EXCEPTION_FLT_DENORMAL_OPERAND");
		exceptionCode_.emplace(EXCEPTION_FLT_DIVIDE_BY_ZERO, L"EXCEPTION_FLT_DIVIDE_BY_ZERO");
		exceptionCode_.emplace(EXCEPTION_FLT_INEXACT_RESULT, L"EXCEPTION_FLT_INEXACT_RESULT");
		exceptionCode_.emplace(EXCEPTION_FLT_INVALID_OPERATION, L"EXCEPTION_FLT_INVALID_OPERATION");
		exceptionCode_.emplace(EXCEPTION_FLT_OVERFLOW, L"EXCEPTION_FLT_OVERFLOW");
		exceptionCode_.emplace(EXCEPTION_FLT_STACK_CHECK, L"EXCEPTION_FLT_STACK_CHECK");
		exceptionCode_.emplace(EXCEPTION_FLT_UNDERFLOW, L"EXCEPTION_FLT_UNDERFLOW");
		exceptionCode_.emplace(EXCEPTION_ILLEGAL_INSTRUCTION, L"EXCEPTION_ILLEGAL_INSTRUCTION");
		exceptionCode_.emplace(EXCEPTION_IN_PAGE_ERROR, L"EXCEPTION_IN_PAGE_ERROR");
		exceptionCode_.emplace(EXCEPTION_INT_DIVIDE_BY_ZERO, L"EXCEPTION_INT_DIVIDE_BY_ZERO");
		exceptionCode_.emplace(EXCEPTION_INT_OVERFLOW, L"EXCEPTION_INT_OVERFLOW");
		exceptionCode_.emplace(EXCEPTION_INVALID_DISPOSITION, L"EXCEPTION_INVALID_DISPOSITION");
		exceptionCode_.emplace(EXCEPTION_NONCONTINUABLE_EXCEPTION, L"EXCEPTION_NONCONTINUABLE_EXCEPTION");
		exceptionCode_.emplace(EXCEPTION_PRIV_INSTRUCTION, L"EXCEPTION_PRIV_INSTRUCTION");
		exceptionCode_.emplace(EXCEPTION_SINGLE_STEP, L"EXCEPTION_SINGLE_STEP");
		exceptionCode_.emplace(EXCEPTION_STACK_OVERFLOW, L"EXCEPTION_STACK_OVERFLOW");
		
		auto cppException = 0xE06D7363;
		exceptionCode_.emplace(cppException, ExceptionCpp);
	}
	
	//-------------------------------------------------------------------------
	ExceptionHandlerStatus ExceptionHandler::HandleException(const EXCEPTION_DEBUG_INFO& exceptionDebugInfo, std::wostream& message)
	{
		const auto& exceptionRecord = exceptionDebugInfo.ExceptionRecord;
				
		if (exceptionDebugInfo.dwFirstChance)
		{
			auto it = breakPointExceptionCode_.find(exceptionRecord.ExceptionCode);
			
			if (it != breakPointExceptionCode_.end())
			{
				// Breakpoint exception need to be ignore the first time
				if (it->second)
					it->second = false; // Mark exception as not first time
				else
					return ExceptionHandlerStatus::BreakPoint;
			}

			return ExceptionHandlerStatus::FirstChanceException;
		}
				
		message << std::endl << std::endl;
		message << L"-----------------------------------------------" << std::endl;
		message << L"*** ";
		message << UnhandledExceptionErrorMessage << exceptionRecord.ExceptionCode;
		message << L": " << GetExceptionStrFromCode(exceptionRecord.ExceptionCode) << std::endl;
		message << L"-----------------------------------------------" << std::endl;

		return ExceptionHandlerStatus::Fatal;
	}

	//-------------------------------------------------------------------------
	std::wstring ExceptionHandler::GetExceptionStrFromCode(DWORD exceptionCode) const
	{
		auto it = exceptionCode_.find(exceptionCode);

		if (it != exceptionCode_.end())
			return it->second;
		
		LPTSTR message = nullptr;
		HMODULE ntDllModule = LoadLibrary(L"NTDLL.DLL");
		Tools::ScopedAction closeLibrary{ [=](){ FreeLibrary(ntDllModule); } };
	
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_FROM_HMODULE,
			ntDllModule,
			exceptionCode,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			reinterpret_cast<LPTSTR>(&message),
			0,
			nullptr);		
		Tools::ScopedAction freeMessage{ [=](){ LocalFree(message); } };

		if (message)
			return message;

		return ExceptionUnknown;
	}
}
