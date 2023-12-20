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

#include "CppCoverage/IDebugEventsHandler.hpp"

namespace CppCoverageTest
{
	class DebugEventsHandlerMock : public CppCoverage::IDebugEventsHandler
	{
	public:
		DebugEventsHandlerMock() = default;

		MOCK_METHOD3(OnCreateProcess, void(HANDLE,const wchar_t*, void*));
		MOCK_METHOD1(OnExitProcess, void(HANDLE));
		MOCK_METHOD3(OnLoadDll, void(HANDLE, HANDLE, const LOAD_DLL_DEBUG_INFO&));
		MOCK_METHOD3(OnUnloadDll, void(HANDLE, HANDLE, const UNLOAD_DLL_DEBUG_INFO&));
		MOCK_METHOD3(OnException, ExceptionType(HANDLE, HANDLE, const EXCEPTION_DEBUG_INFO&));

	private:
		DebugEventsHandlerMock(const DebugEventsHandlerMock&) = delete;
		DebugEventsHandlerMock& operator=(const DebugEventsHandlerMock&) = delete;
	};
}