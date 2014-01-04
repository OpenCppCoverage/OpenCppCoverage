#ifndef CPPCOVERAGE_IDEBUGINFORMATIONEVENTHANDLER_HEADER_GARD
#define CPPCOVERAGE_IDEBUGINFORMATIONEVENTHANDLER_HEADER_GARD

namespace CppCoverage
{
	class IDebugInformationEventHandler
	{
	public:
		IDebugInformationEventHandler() = default;
		virtual ~IDebugInformationEventHandler() = default;
								
		virtual bool IsSourceFileSelected(const std::string& filename) const = 0;
		virtual void OnNewLine(const std::string& fileName, int lineNumber, DWORD64 address) = 0;
		
	private:
		IDebugInformationEventHandler(const IDebugInformationEventHandler&) = delete;
		IDebugInformationEventHandler& operator=(const IDebugInformationEventHandler&) = delete;
	};
}

#endif
