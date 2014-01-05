#ifndef CPPCOVERAGE_DEBUGGER_HEADER_GARD
#define CPPCOVERAGE_DEBUGGER_HEADER_GARD

#include "Export.hpp"

namespace CppCoverage
{
	class StartInfo;
	class IDebugEventsHandler;

	class DLL Debugger
	{
	public:
		Debugger() = default;

		void Debug(const StartInfo&, IDebugEventsHandler&);

	private:
		Debugger(const Debugger&) = delete;
		Debugger& operator=(const Debugger&) = delete;
	};
}

#endif
