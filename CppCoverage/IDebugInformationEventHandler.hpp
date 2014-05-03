#pragma once

#include <string>
#include <Windows.h>
#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL IDebugInformationEventHandler
	{
	public:
		IDebugInformationEventHandler() = default;
		virtual ~IDebugInformationEventHandler() = default;
								
		virtual bool IsSourceFileSelected(const std::wstring& filename) const = 0;
		virtual void OnNewLine(const std::wstring& fileName, int lineNumber, DWORD64 address) = 0;
		
	private:
		IDebugInformationEventHandler(const IDebugInformationEventHandler&) = delete;
		IDebugInformationEventHandler& operator=(const IDebugInformationEventHandler&) = delete;
	};
}


