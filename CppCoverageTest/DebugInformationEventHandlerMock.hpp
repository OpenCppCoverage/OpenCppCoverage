#pragma once

#include "CppCoverage/IDebugInformationEventHandler.hpp"

namespace CppCoverageTest
{
	class DebugInformationEventHandlerMock : public CppCoverage::IDebugInformationEventHandler
	{
	public:
		DebugInformationEventHandlerMock() = default;
		
		MOCK_CONST_METHOD1(IsSourceFileSelected, bool(const std::wstring&));
		MOCK_METHOD3(OnNewLine, void(const std::wstring& fileName, int lineNumber, DWORD64 address));

	private:
		DebugInformationEventHandlerMock(const DebugInformationEventHandlerMock&) = delete;
		DebugInformationEventHandlerMock& operator=(const DebugInformationEventHandlerMock&) = delete;
	};
}


