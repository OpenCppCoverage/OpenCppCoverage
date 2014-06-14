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
		class ExceptionHandlerTest : public ::testing::Test, private CppCoverage::IDebugEventsHandler
		{
		public:								
			void Run(const std::wstring& commandLineArgument)
			{
				cov::StartInfo startInfo{ TestCoverageConsole::GetOutputBinaryPath() };
				cov::Debugger debugger;

				startInfo.AddArguments(commandLineArgument);
				debugger.Debug(startInfo, *this);
			}

			virtual DWORD OnException(HANDLE hProcess, HANDLE hThread, const EXCEPTION_DEBUG_INFO& exceptionDebugInfo) override			
			{							
				std::wostringstream ostr;

				exceptionHandlerStatus_ = handler_.HandleException(exceptionDebugInfo, ostr);
				message_ = ostr.str();

				return DBG_EXCEPTION_NOT_HANDLED;
			}
			
			CppCoverage::ExceptionHandlerStatus exceptionHandlerStatus_;
			std::wstring message_;
			cov::ExceptionHandler handler_;
		};

		//-----------------------------------------------------------------------------
		std::wstring GetErrorMessage(int errorCode)
		{
			cov::ExceptionHandler handler;
			EXCEPTION_DEBUG_INFO exceptionDebugInfo{};
			std::wostringstream ostr;

			exceptionDebugInfo.ExceptionRecord.ExceptionCode = errorCode;
			handler.HandleException(exceptionDebugInfo, ostr); // skip first exception
			handler.HandleException(exceptionDebugInfo, ostr);

			return ostr.str();
		};
	}
	
	//-----------------------------------------------------------------------------
	TEST_F(ExceptionHandlerTest, TestUnHandleCppException)
	{
		Run(TestCoverageConsole::TestThrowUnHandledCppException);
		ASSERT_EQ(cov::ExceptionHandlerStatus::Fatal, exceptionHandlerStatus_);
		ASSERT_NE(std::string::npos, message_.find(cov::ExceptionHandler::ExceptionCpp));
	}

	//-----------------------------------------------------------------------------
	TEST_F(ExceptionHandlerTest, TestUnHandleSEHException)
	{
		Run(TestCoverageConsole::TestThrowUnHandledSEHException);	
		ASSERT_EQ(CppCoverage::ExceptionHandlerStatus::Fatal, exceptionHandlerStatus_);
		ASSERT_NE(std::string::npos, message_.find(cov::ExceptionHandler::ExceptionAccesViolation));
	}
	
	//-----------------------------------------------------------------------------
	TEST_F(ExceptionHandlerTest, TestFormatMessage)
	{
		auto message = GetErrorMessage(cov::ExceptionHandler::ExceptionEmulationX86ErroCode);
		ASSERT_EQ(CppCoverage::ExceptionHandlerStatus::Fatal, exceptionHandlerStatus_);
		ASSERT_NE(std::string::npos, message.find(L"Exception status code used by Win32 x86 emulation subsystem"));
	}

	//-----------------------------------------------------------------------------
	TEST_F(ExceptionHandlerTest, TestUnknown)
	{
		auto message = GetErrorMessage(42);
		ASSERT_EQ(CppCoverage::ExceptionHandlerStatus::Fatal, exceptionHandlerStatus_);
		ASSERT_NE(std::string::npos, message.find(cov::ExceptionHandler::ExceptionUnknown));
	}
}