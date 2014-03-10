#pragma once

namespace CppCoverage
{
	class BreakPoint
	{
	public:
		explicit BreakPoint(HANDLE hProcess);

		unsigned char SetBreakPointAt(void* address) const;
		void RemoveBreakPoint(void* address, unsigned char oldInstruction) const;

		void AdjustEipAfterBreakPointRemoval(HANDLE hThread) const;

	private:
		BreakPoint(const BreakPoint&) = delete;
		BreakPoint& operator=(const BreakPoint&) = delete;

		unsigned char ReadProcessMemory(void* address) const;
		void WriteProcessMemory(void* address, unsigned char instruction) const;

	private:
		HANDLE hProcess_;
	};
}


