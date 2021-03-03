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

#include "CppCoverage/ExceptionHandler.hpp"
#include "CppCoverage/Debugger.hpp"
#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/IDebugEventsHandler.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{

	namespace
	{
		//---------------------------------------------------------------------
		class ExceptionHandlerTest : public ::testing::Test, private CppCoverage::IDebugEventsHandler
		{
		public:
			//---------------------------------------------------------------------
			void Run(const std::wstring& commandLineArgument)
			{
				cov::StartInfo startInfo{ TestCoverageConsole::GetOutputBinaryPath() };
				cov::Debugger debugger{ false, false, false, false };

				startInfo.AddArgument(commandLineArgument);
				debugger.Debug(startInfo, *this);
			}

			//---------------------------------------------------------------------
			virtual ExceptionType OnException(HANDLE hProcess, HANDLE hThread, const EXCEPTION_DEBUG_INFO& exceptionDebugInfo) override
			{							
				std::wostringstream ostr;

				exceptionHandlerStatus_ = handler_.HandleException(hProcess, exceptionDebugInfo, ostr);
				message_ = ostr.str();

				return IDebugEventsHandler::ExceptionType::NotHandled;;
			}
			
			//-----------------------------------------------------------------
			std::wstring HandleExceptionTwice(int errorCode, bool firstChange)
			{
				auto exceptionDebugInfo = CreateExceptionDebugInfo(errorCode, firstChange);

				handler_.HandleException(nullptr, exceptionDebugInfo, ostr_); // skip first exception
				handler_.HandleException(nullptr, exceptionDebugInfo, ostr_);

				return ostr_.str();
			};

			//-----------------------------------------------------------------------------
			EXCEPTION_DEBUG_INFO CreateExceptionDebugInfo(
				int errorCode = cov::ExceptionHandler::ExceptionEmulationX86ErrorCode,
				bool firstChange = true)
			{
				EXCEPTION_DEBUG_INFO exceptionDebugInfo{};

				exceptionDebugInfo.dwFirstChance = firstChange ? 1 : 0;
				exceptionDebugInfo.ExceptionRecord.ExceptionCode = errorCode;

				return exceptionDebugInfo;
			}

			CppCoverage::ExceptionHandlerStatus exceptionHandlerStatus_;
			std::wstring message_;
			cov::ExceptionHandler handler_;
			std::wostringstream ostr_;
		};
	}

	//-----------------------------------------------------------------------------
	TEST_F(ExceptionHandlerTest, TestUnHandleCppException)
	{
		Run(TestCoverageConsole::TestThrowUnHandledCppException);
		ASSERT_EQ(cov::ExceptionHandlerStatus::CppError, exceptionHandlerStatus_);
		ASSERT_NE(std::string::npos, message_.find(cov::ExceptionHandler::ExceptionCpp));
	}

	//-----------------------------------------------------------------------------
	TEST_F(ExceptionHandlerTest, TestUnHandleSEHException)
	{
		Run(TestCoverageConsole::TestThrowUnHandledSEHException);	
		ASSERT_EQ(CppCoverage::ExceptionHandlerStatus::Error, exceptionHandlerStatus_);
		ASSERT_NE(std::string::npos, message_.find(cov::ExceptionHandler::ExceptionAccesViolation));
	}
	
	//-----------------------------------------------------------------------------
	TEST_F(ExceptionHandlerTest, TestFormatMessage)
	{
		auto message = HandleExceptionTwice(cov::ExceptionHandler::ExceptionEmulationX86ErrorCode, false);
		ASSERT_NE(std::string::npos, message.find(L"Exception status code used by Win32 x86 emulation subsystem"));
	}

	//-----------------------------------------------------------------------------
	TEST_F(ExceptionHandlerTest, TestUnknown)
	{
		auto message = HandleExceptionTwice(42, false);
		ASSERT_NE(std::string::npos, message.find(cov::ExceptionHandler::ExceptionUnknown));
	}

	//-----------------------------------------------------------------------------
	TEST_F(ExceptionHandlerTest, ChildProcess)
	{
		auto exceptionDebugInfo = CreateExceptionDebugInfo();

		ASSERT_EQ(cov::ExceptionHandlerStatus::FirstChanceException,
			handler_.HandleException(nullptr, exceptionDebugInfo, ostr_));
		ASSERT_EQ(cov::ExceptionHandlerStatus::BreakPoint,
			handler_.HandleException(nullptr, exceptionDebugInfo, ostr_));
		ASSERT_EQ(cov::ExceptionHandlerStatus::FirstChanceException,
			handler_.HandleException(reinterpret_cast<HANDLE>(42), exceptionDebugInfo, ostr_));
	}

	//-----------------------------------------------------------------------------
	TEST_F(ExceptionHandlerTest, ExitProcess)
	{
		auto exceptionDebugInfo = CreateExceptionDebugInfo();
		const HANDLE handle = nullptr;
		const auto handle2 = reinterpret_cast<HANDLE>(42);
		ASSERT_EQ(cov::ExceptionHandlerStatus::FirstChanceException,
			handler_.HandleException(handle, exceptionDebugInfo, ostr_));
		ASSERT_EQ(cov::ExceptionHandlerStatus::FirstChanceException,
			handler_.HandleException(handle2, exceptionDebugInfo, ostr_));

		handler_.OnExitProcess(handle2);
		ASSERT_EQ(cov::ExceptionHandlerStatus::FirstChanceException,
			handler_.HandleException(handle2, exceptionDebugInfo, ostr_));

		handler_.OnExitProcess(handle);
		ASSERT_EQ(cov::ExceptionHandlerStatus::FirstChanceException,
			handler_.HandleException(handle, exceptionDebugInfo, ostr_));
	}
}