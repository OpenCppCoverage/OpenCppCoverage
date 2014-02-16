#ifndef CPPCOVERAGE_IDEBUGINFORMATIONEVENTHANDLER_HEADER_GARD
#define CPPCOVERAGE_IDEBUGINFORMATIONEVENTHANDLER_HEADER_GARD

#include <string>

#include "Export.hpp"

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

#endif
