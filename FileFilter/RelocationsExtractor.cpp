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
#include <memory>
#include "FileFilterException.hpp"
#include "Tools/ProcessMemory.hpp"
#include "Tools/PEFileHeader.hpp"

namespace FileFilter
{
	namespace
	{
		//-------------------------------------------------------------------------
		DWORD64 ExtractRelocations(
			HANDLE hProcess,
			DWORD64 baseOfImage,
			DWORD64 imageBaseRelocationPtr,
			int sizeOfPointer,
			std::unordered_set<DWORD64>& relocations)
		{
			auto imageBaseRelocation = Tools::ReadStructInProcessMemory<IMAGE_BASE_RELOCATION>(
				hProcess, imageBaseRelocationPtr);
			auto sizeOfBlock = imageBaseRelocation->SizeOfBlock;
			auto count = (sizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

			std::vector<WORD> relocationPtrs(count);
			Tools::ReadProcessMemory(
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
					DWORD_PTR relocationValue = 0;
					Tools::ReadProcessMemory(hProcess, relocationAddress, &relocationValue, sizeOfPointer);

					auto relocation = relocationValue - baseOfImage;
					relocations.insert(relocation);
				}
			}

			return sizeOfBlock;
		}

		//-------------------------------------------------------------------------
		struct RelocationsDirectoryInfo
		{
			IMAGE_DATA_DIRECTORY directory;
			int sizeOfPointer;
		};

		//-------------------------------------------------------------------------
		template <typename T_IMAGE_NT_HEADERS>
		std::unique_ptr<RelocationsDirectoryInfo>
			GetRelocationsDirectory(const T_IMAGE_NT_HEADERS& ntHeaders, int sizeOfPointer)
		{
			auto relocationsDirectoryInfo = std::make_unique<RelocationsDirectoryInfo>();

			const auto& optionalHeader = ntHeaders.OptionalHeader;
			relocationsDirectoryInfo->directory =
				optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
			relocationsDirectoryInfo->sizeOfPointer = sizeOfPointer;

			return relocationsDirectoryInfo;
		}

		//-------------------------------------------------------------------------
		struct PEFileHeaderHandler : public Tools::IPEFileHeaderHandler
		{
			//-----------------------------------------------------------------
			void OnNtHeader32(HANDLE hProcess,
			                  DWORD64 baseOfImage,
			                  const IMAGE_NT_HEADERS32& ntHeader) override
			{
				FillRelocations(
				    hProcess,
				    baseOfImage,
				    GetRelocationsDirectory(ntHeader, sizeof(DWORD)));
			}

			//-------------------------------------------------------------------------
			void OnNtHeader64(HANDLE hProcess,
			                  DWORD64 baseOfImage,
			                  const IMAGE_NT_HEADERS64& ntHeader) override
			{
				FillRelocations(
				    hProcess,
				    baseOfImage,
				    GetRelocationsDirectory(ntHeader, sizeof(DWORD_PTR)));
			}

			//-----------------------------------------------------------------
			void FillRelocations(
			    HANDLE hProcess,
			    DWORD64 baseOfImage,
			    std::unique_ptr<RelocationsDirectoryInfo> relocationsInfo)
			{
				const auto& directory = relocationsInfo->directory;
				auto imageBaseRelocationPtr =
				    baseOfImage + directory.VirtualAddress;
				auto endBaseRelocationPtr =
				    imageBaseRelocationPtr + directory.Size;

				while (imageBaseRelocationPtr < endBaseRelocationPtr)
				{
					imageBaseRelocationPtr +=
					    ExtractRelocations(hProcess,
					                       baseOfImage,
					                       imageBaseRelocationPtr,
					                       relocationsInfo->sizeOfPointer,
					                       relocations_);
				}
			}

			std::unordered_set<DWORD64> relocations_;
		};
	}

	//-------------------------------------------------------------------------
	std::unordered_set<DWORD64>
	RelocationsExtractor::Extract(HANDLE hProcess, DWORD64 baseOfImage) const
	{
		Tools::PEFileHeader peFileHeader;
		PEFileHeaderHandler handler;

		peFileHeader.Load(hProcess, baseOfImage, handler);

		return std::move(handler.relocations_);
	}
}
