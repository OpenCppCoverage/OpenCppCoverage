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
#include "Address.hpp"

#include "Tools/Log.hpp"

namespace CppCoverage
{
	namespace
	{
		//-------------------------------------------------------------------------
		std::vector<unsigned char>
		ReadProcessMemory(HANDLE hProcess, void* address, size_t size)
		{
			std::vector<unsigned char> data(size);
			SIZE_T totalWritten = 0;
			SIZE_T written = 0;

			while (totalWritten < size)
			{
				if (!::ReadProcessMemory(hProcess,
				                         address,
				                         &data[totalWritten],
				                         data.size() - totalWritten,
				                         &written))
				{
					LOG_ERROR << "Cannot read memory";
				}
				totalWritten += written;
			}

			return data;
		}

		//-------------------------------------------------------------------------
		void WriteProcessMemory(HANDLE hProcess,
		                        void* address,
		                        void* buffer,
		                        size_t size)
		{
			SIZE_T totalWritten = 0;
			SIZE_T written = 0;

			while (totalWritten < size)
			{
				auto startBuffer = static_cast<char*>(buffer) + totalWritten;
				if (!::WriteProcessMemory(hProcess,
				                          address,
				                          startBuffer,
				                          size - totalWritten,
				                          &written))
				{
					LOG_ERROR << "Cannot write memory:";
				}

				if (!FlushInstructionCache(hProcess, startBuffer, written))
					THROW_LAST_ERROR("Cannot flush memory:" << address,
					                 GetLastError());
				totalWritten += written;
			}
		}
	}

	using Addresses = std::vector<DWORD64>;
	using AddressesIt = Addresses::const_iterator;

	//-------------------------------------------------------------------------
	void SetBreakPointsRange(HANDLE hProcess,
	                         AddressesIt begin,
	                         AddressesIt end,
	                         BreakPoint::InstructionCollection& oldInstructions)
	{
		if (begin == end)
			return;

		auto firstValue = *begin;
		auto memorySpaceSize =
		    *(end - 1) - firstValue + sizeof(BreakPoint::breakPointInstruction);
		auto firstAddress = reinterpret_cast<void*>(firstValue);
		auto buffer = ReadProcessMemory(
		    hProcess, firstAddress, static_cast<size_t>(memorySpaceSize));

		for (auto it = begin; it < end; ++it)
		{
			auto index = static_cast<size_t>(*it - firstValue);
			auto oldInstruction = buffer[index];
			buffer[index] = BreakPoint::breakPointInstruction;
			oldInstructions.emplace_back(oldInstruction, *it);
		}
		WriteProcessMemory(hProcess, firstAddress, &buffer[0], buffer.size());
	}

	const unsigned char BreakPoint::breakPointInstruction = 0xCC;

	//-------------------------------------------------------------------------
	BreakPoint::InstructionCollection
	BreakPoint::SetBreakPoints(HANDLE hProcess, Addresses&& addresses) const
	{
		InstructionCollection oldInstructions;

		std::sort(addresses.begin(), addresses.end());
		auto beginRange = addresses.cbegin();

		for (auto it = beginRange; it < addresses.cend(); ++it)
		{
			if (*it - *beginRange > 4096)
			{
				SetBreakPointsRange(hProcess, beginRange, it, oldInstructions);
				beginRange = it;
			}
		}
		SetBreakPointsRange(
		    hProcess, beginRange, addresses.end(), oldInstructions);

		return oldInstructions;
	}

	//-------------------------------------------------------------------------
	void BreakPoint::RemoveBreakPoint(const Address& address,
	                                  unsigned char oldInstruction) const
	{
		WriteProcessMemory(address.GetProcessHandle(),
		                   address.GetValue(),
		                   &oldInstruction,
		                   sizeof(oldInstruction));
	}

	//-------------------------------------------------------------------------
	void BreakPoint::AdjustEipAfterBreakPointRemoval(HANDLE hThread) const
	{
		CONTEXT lcContext;
		lcContext.ContextFlags = CONTEXT_ALL;
		if (!GetThreadContext(hThread, &lcContext))
			THROW_LAST_ERROR("Error in GetThreadContext", GetLastError());

#ifdef _WIN64
		--lcContext.Rip; // Move back one byte
#else
		--lcContext.Eip; // Move back one byte
#endif
		if (!SetThreadContext(hThread, &lcContext))
			THROW_LAST_ERROR("Error in SetThreadContext", GetLastError());
	}
}
