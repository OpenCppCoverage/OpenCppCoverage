#ifndef CPPCOVERAGE_DEBUGINFORMATIONEVENTHANDLER_HEADER_GARD
#define CPPCOVERAGE_DEBUGINFORMATIONEVENTHANDLER_HEADER_GARD

#include "IDebugInformationEventHandler.hpp"

namespace CppCoverage
{
	class DebugInformationEventHandler : IDebugInformationEventHandler
	{
	public:
		DebugInformationEventHandler() = default;
	private:
		DebugInformationEventHandler(const DebugInformationEventHandler&) = delete;
		DebugInformationEventHandler& operator=(const DebugInformationEventHandler&) = delete;
	};
}

#endif
