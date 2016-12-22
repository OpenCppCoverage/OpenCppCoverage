// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage
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
#include "RelocationsExtractor.hpp"
#include "FileFilterException.hpp"

namespace FileFilter
{			
	//-------------------------------------------------------------------------
	const IMAGE_NT_HEADERS& GetNTHeader(const void* baseOfImage)
	{
		auto* dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(baseOfImage);

		if (dosHeader == nullptr || dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			THROW("The image is not a valid NT headers.");

		auto* dosHeaderAddress = reinterpret_cast<const char*>(dosHeader);
		
		return *reinterpret_cast<const IMAGE_NT_HEADERS*>(dosHeaderAddress + dosHeader->e_lfanew);
	}

	//-------------------------------------------------------------------------
	DWORD_PTR ToPtr(DWORD_PTR value)
	{
		if (!value)
			THROW("Null ptr");
		return *reinterpret_cast<DWORD_PTR*>(value);
	}

	//-------------------------------------------------------------------------
	DWORD_PTR ExtractRelocationsAddress(
		DWORD_PTR baseRelocationPtr,
		DWORD_PTR moduleAddress,
		DWORD_PTR addressOffset,
		std::unordered_set<DWORD_PTR>& relocations)
	{
		if (!baseRelocationPtr)
			THROW("Invalid base relocation");

		auto& imageBaseRelocation = *reinterpret_cast<IMAGE_BASE_RELOCATION*>(baseRelocationPtr);		
		auto relocation = baseRelocationPtr + sizeof(IMAGE_BASE_RELOCATION);
		auto count = (imageBaseRelocation.SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
		
		for (unsigned int i = 0; i < count; ++i)
		{
			auto relocationPtr = ToPtr(relocation);
			auto relocationType = (relocationPtr & 0xf000) >> 12;

			if (relocationType == IMAGE_REL_BASED_HIGHLOW || relocationType == IMAGE_REL_BASED_DIR64)
			{
				auto rva = relocationPtr & 0x0fff;
				auto relocationValueAddress = imageBaseRelocation.VirtualAddress + rva + moduleAddress;
				auto relocationValue = ToPtr(relocationValueAddress) - addressOffset;
				
				relocations.insert(relocationValue);				
			}
			relocation += sizeof(WORD);
		}
		return relocation;
	}

	//-------------------------------------------------------------------------
	std::unordered_set<DWORD_PTR> RelocationsExtractor::Extract(
		HANDLE hFile, 
		void* baseOfImage) const
	{		
		const auto& ntHeader = GetNTHeader(baseOfImage);
		const auto& optionalHeader = ntHeader.OptionalHeader;		
		const auto& directory = optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
		auto moduleAddress = reinterpret_cast<DWORD_PTR>(hFile);
		auto baseRelocationPtr = moduleAddress + directory.VirtualAddress;
		auto endBaseRelocation = baseRelocationPtr + directory.Size;
		auto addressOffset = moduleAddress - optionalHeader.ImageBase;
		
		std::unordered_set<DWORD_PTR> relocations;
		while (baseRelocationPtr && baseRelocationPtr < endBaseRelocation)
		{
			baseRelocationPtr = ExtractRelocationsAddress(
				baseRelocationPtr, moduleAddress, addressOffset, relocations);
		}
		
		return relocations;
	}
}
