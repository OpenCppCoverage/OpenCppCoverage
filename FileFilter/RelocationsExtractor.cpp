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
#include <memory>

namespace FileFilter
{
	namespace
	{
		//-------------------------------------------------------------------------
		void ReadProcessMemory(
			HANDLE hProcess,
			DWORD64 address,
			void* buffer,
			SIZE_T size)
		{
			SIZE_T bytesRead = 0;

			if (!::ReadProcessMemory(hProcess,
				reinterpret_cast<void*>(address),
				buffer,
				size,
				&bytesRead) || bytesRead != size)
			{
				THROW("Cannot read process memory");
			}
		}

		//-------------------------------------------------------------------------
		template <typename T>
		T ReadProcessMemory(
			HANDLE hProcess,
			DWORD64 address)
		{
			T data;
			ReadProcessMemory(hProcess, address, &data, sizeof(T));
			return data;
		}

		//-------------------------------------------------------------------------
		template<typename T>
		std::unique_ptr<T> ReadStructInProcessMemory(
			HANDLE hProcess,
			DWORD64 address)
		{
			auto data = std::make_unique<T>();
			ReadProcessMemory(hProcess, address, data.get(), sizeof(T));

			return data;
		}

		//-------------------------------------------------------------------------
		DWORD64 ExtractRelocations(
			HANDLE hProcess,
			DWORD64 baseOfImage,
			DWORD64 baseAddress,
			DWORD64 imageBaseRelocationPtr,
			std::unordered_set<DWORD64>& relocations)
		{
			auto imageBaseRelocation = ReadStructInProcessMemory<IMAGE_BASE_RELOCATION>(
				hProcess, imageBaseRelocationPtr);
			auto sizeOfBlock = imageBaseRelocation->SizeOfBlock;
			auto count = (sizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

			std::vector<WORD> relocationPtrs(count);
			ReadProcessMemory(
				hProcess,
				imageBaseRelocationPtr + sizeof(IMAGE_BASE_RELOCATION),
				&relocationPtrs[0],
				relocationPtrs.size() * sizeof(WORD));

			for (auto relocationPtr : relocationPtrs)
			{
				auto relocationType = (relocationPtr & 0xf000) >> 12;

				if (relocationType == IMAGE_REL_BASED_HIGHLOW || relocationType == IMAGE_REL_BASED_DIR64)
				{
					auto rva = relocationPtr & 0x0fff;
					auto relocationAddress = imageBaseRelocation->VirtualAddress + rva + baseOfImage;
					auto relocationValue = ReadProcessMemory<DWORD_PTR>(hProcess, relocationAddress);

					auto relocation = relocationValue + baseAddress - baseOfImage;
					relocations.insert(relocation);
				}
			}

			return sizeOfBlock;
		}
	}

	//-------------------------------------------------------------------------
	std::unordered_set<DWORD64> RelocationsExtractor::Extract(
		HANDLE hProcess,
		DWORD64 baseOfImage,
		DWORD64 baseAddress) const
	{
		auto dosHeader = ReadStructInProcessMemory<IMAGE_DOS_HEADER>(hProcess, baseOfImage);
		if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			THROW("The image is not a valid DOS image.");

		auto ntHeader = ReadStructInProcessMemory<IMAGE_NT_HEADERS>(
			hProcess, baseOfImage + dosHeader->e_lfanew);
	
		const auto& optionalHeader = ntHeader->OptionalHeader;
		const auto& directory = optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

		std::unordered_set<DWORD64> relocations;
		auto imageBaseRelocationPtr = baseOfImage + directory.VirtualAddress;
		auto endBaseRelocationPtr = imageBaseRelocationPtr + directory.Size;

		while (imageBaseRelocationPtr < endBaseRelocationPtr)
		{
			imageBaseRelocationPtr += ExtractRelocations(
				hProcess,
				baseOfImage,
				baseAddress,
				imageBaseRelocationPtr,
				relocations);
		}

		return relocations;
	}
}
