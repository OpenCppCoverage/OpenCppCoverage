// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
	void BreakPoint::AdjustEipAfterBreakPointRemoval(HANDLE hThread) const
	{
		CONTEXT lcContext;
		lcContext.ContextFlags = CONTEXT_ALL;
		GetThreadContext(hThread, &lcContext);
		lcContext.Eip--; // Move back one byte
		SetThreadContext(hThread, &lcContext);
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
