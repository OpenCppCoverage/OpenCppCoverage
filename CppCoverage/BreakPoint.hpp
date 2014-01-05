#ifndef CPPCOVERAGE_BREAKPOINT_HEADER_GARD
#define CPPCOVERAGE_BREAKPOINT_HEADER_GARD

namespace CppCoverage
{
	class BreakPoint
	{
	public:
		explicit BreakPoint(HANDLE hProcess);

		unsigned char SetBreakPointAt(void* address) const;
		void RemoveBreakPoint(void* address, unsigned char oldInstruction) const;

	private:
		BreakPoint(const BreakPoint&) = delete;
		BreakPoint& operator=(const BreakPoint&) = delete;

		unsigned char ReadProcessMemory(void* address) const;
		void WriteProcessMemory(void* address, unsigned char instruction) const;

	private:
		HANDLE hProcess_;
	};
}

#endif
