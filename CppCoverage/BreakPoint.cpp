#include "stdafx.h"
#include "BreakPoint.hpp"

#include "CppCoverageException.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	BreakPoint::BreakPoint(HANDLE hProcess)
		: hProcess_(hProcess)
	{
	}

	//-------------------------------------------------------------------------
	unsigned char BreakPoint::SetBreakPointAt(void* address) const
	{
		auto instruction = ReadProcessMemory(address);
		unsigned char breakPointInstruction = 0xCC;

		WriteProcessMemory(address, breakPointInstruction);

		return instruction;
	}

	//-------------------------------------------------------------------------
	void BreakPoint::RemoveBreakPoint(void* address, unsigned char oldInstruction) const
	{
		WriteProcessMemory(address, oldInstruction);
	}

	//-------------------------------------------------------------------------
	unsigned char BreakPoint::ReadProcessMemory(void* address) const
	{
		unsigned char instruction = 0;
		SIZE_T written = 0;

		if (!::ReadProcessMemory(hProcess_, address, &instruction, sizeof(instruction), &written))
			THROW_LAST_ERROR("Cannot read memory:" << address, GetLastError());

		return instruction;
	}

	//-------------------------------------------------------------------------
	void BreakPoint::WriteProcessMemory(void* address, unsigned char instruction) const
	{	
		SIZE_T written = 0;
		if (!::WriteProcessMemory(hProcess_, address, &instruction, sizeof(instruction), &written))
			THROW_LAST_ERROR("Cannot write memory:" << address, GetLastError());			

		if (!FlushInstructionCache(hProcess_, address, sizeof(instruction)))
			THROW_LAST_ERROR("Cannot flush memory:" << address, GetLastError());
	}
}
