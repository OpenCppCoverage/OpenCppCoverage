#ifndef CPPCOVERAGE_DEBUGGER_HEADER_GARD
#define CPPCOVERAGE_DEBUGGER_HEADER_GARD

#include "Export.hpp"

namespace CppCoverage
{
	class StartInfo;
	class IDebugEvents;

	class DLL Debugger
	{
	public:
		Debugger() = default;

		void Debug(const StartInfo&, IDebugEvents&);

	private:
		Debugger(const Debugger&) = delete;
		Debugger& operator=(const Debugger&) = delete;
	};
}

#endif
