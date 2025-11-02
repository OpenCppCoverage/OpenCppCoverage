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
#include "HandleInformation.hpp"

#include <Psapi.h>
#include <boost/algorithm/string.hpp>
#include "CppCoverageException.hpp"
#include "Handle.hpp"

namespace CppCoverage
{
	namespace
	{
		//-------------------------------------------------------------------------
		std::vector<std::wstring> GetLogicalDrives()
		{
			std::vector<wchar_t> logicalDriveStrings(PathBufferSize);
			std::vector<std::wstring> logicalDrives;

			auto size = GetLogicalDriveStrings(
				static_cast<int>(logicalDriveStrings.size()), 
				&logicalDriveStrings[0]);

			if (!size)
				THROW(L"Cannot GetLogicalDriveStrings");

			size_t startIndex = 0;

			for (size_t i = 0; i < size; ++i)
			{
				if (!logicalDriveStrings[i])
				{
					std::wstring localDrive(&logicalDriveStrings[startIndex], &logicalDriveStrings[i]);
					logicalDrives.push_back(localDrive);
					startIndex = i + 1;
				}
			}

			return logicalDrives;
		}

		//-------------------------------------------------------------------------
		std::vector<std::pair<std::wstring, std::wstring>> GetQueryDosDevicesMapping()
		{
			std::vector<wchar_t> dosDevice(PathBufferSize);
			std::vector<std::pair<std::wstring, std::wstring>> queryDosDevicesMapping;

			for (const auto& logicalDrive : GetLogicalDrives())
			{
				auto pos = logicalDrive.find('\\');
				std::wstring drive = (pos != std::string::npos) ? logicalDrive.substr(0, pos) : logicalDrive;

				if (QueryDosDevice(drive.c_str(), &dosDevice[0], static_cast<int>(dosDevice.size())))
				{
					std::wstring dosDeviceName{ &dosDevice[0] };
					queryDosDevicesMapping.emplace_back(dosDeviceName, drive);
				}
			}

			// Handle network drive. We just remove prefix.
			queryDosDevicesMapping.emplace_back(L"\\Device\\Mup", L"\\");
			return queryDosDevicesMapping;
		}

		//-------------------------------------------------------------------------
		std::wstring GetMappedFileNameStr(HANDLE hfile)
		{
			DWORD dwFileSizeHi = 0;
			DWORD dwFileSizeLo = GetFileSize(hfile, &dwFileSizeHi);

			if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
				THROW(L"Cannot map a file with a length of zero.");

			auto fileMappingHandle = CreateHandle(
				CreateFileMapping(hfile, NULL, PAGE_READONLY, 0, 1, NULL),
				CloseHandle);

			auto mapViewOfFile = CreateHandle(
				MapViewOfFile(fileMappingHandle.GetValue(), FILE_MAP_READ, 0, 0, 1),
				UnmapViewOfFile);

			TCHAR pszFilename[MAX_PATH + 1];

			if (!GetMappedFileName(GetCurrentProcess(),
				mapViewOfFile.GetValue(),
				pszFilename,
				MAX_PATH))
			{
				THROW(L"Cannot GetMappedFileName");
			}

			return pszFilename;
		}

		//-------------------------------------------------------------------------
		std::wstring GetFinalPathName(HANDLE hfile)
		{
			std::vector<wchar_t> buffer(PathBufferSize);

			if (GetFinalPathNameByHandle(hfile, &buffer[0], static_cast<int>(buffer.size()) - 1, VOLUME_NAME_NT))
				return &buffer[0];

			return GetMappedFileNameStr(hfile);
		}
	}

	//-------------------------------------------------------------------------
	HandleInformation::HandleInformation() = default;

	//-------------------------------------------------------------------------
	std::wstring HandleInformation::ComputeFilename(HANDLE hfile) const
	{
		if (hfile == nullptr)
		{
			THROW(L"OpenCppCoverage cannot find the name of the module.\n"
			      L"See "
			      L"https://github.com/OpenCppCoverage/OpenCppCoverage/wiki/"
			      L"FAQ#error-opencppcoverage-cannot-find-the-name-of-the-"
			      L"module for more information.");
		}

		auto mappedFileName = GetFinalPathName(hfile);
		auto queryDosDevicesMapping = GetQueryDosDevicesMapping();

		for (const auto& queryDosDeviceMapping : queryDosDevicesMapping)
		{
			const auto& deviceName = queryDosDeviceMapping.first;
			const auto& logicalDrive = queryDosDeviceMapping.second;

			if (boost::algorithm::istarts_with(mappedFileName, deviceName))
			{
				boost::algorithm::ireplace_first(mappedFileName, deviceName, logicalDrive);
				return mappedFileName;
			}
		}

		THROW(L"Cannot find path for the handle: " << mappedFileName);
	}

	//-------------------------------------------------------------------------
	std::wstring HandleInformation::ComputeFilename(
		HANDLE hfile,
		HANDLE hprocess) const
	{
		if (hfile != nullptr)
		{
			return ComputeFilename(hfile);
		}

		// Windows API doesn't give us a correct hfile HANDLE, as it does with container execution
		// Trying to bypass it
		if (hprocess != nullptr)
		{
			TCHAR moduleName[MAX_PATH] = {};
			if (GetModuleFileNameEx(
				hprocess,
				NULL,
				moduleName,
				sizeof(moduleName) / sizeof(TCHAR)
			))
			{
				return std::wstring(moduleName);
			}
		}

		// We can't bypass it
		// Calling default method to give the correct message
		return ComputeFilename(hfile);
	}

	//-------------------------------------------------------------------------
	std::wstring HandleInformation::ComputeFilename(
		HANDLE hfile,
		HANDLE hprocess,
		LPVOID lpimagename,
		WORD wunicode) const
	{
		if (hfile != nullptr)
		{
			return ComputeFilename(hfile);
		}

		// Windows API doesn't give us a correct hfile HANDLE, as it does with container execution
		// Trying to bypass it
		if (hprocess != nullptr && lpimagename != nullptr)
		{
			LPVOID lpnameinhprocess = nullptr;
			BOOL success = FALSE;

			success = ReadProcessMemory(
				hprocess,
				lpimagename,
				&lpnameinhprocess,
				sizeof(lpnameinhprocess),
				NULL);

			if (success != FALSE && lpnameinhprocess != nullptr)
			{
				std::wstring mappedFileName;
				wchar_t wbuffer[MAX_PATH] = {};

				if (wunicode == 0) // We use ANSI format
				{
					char buffer[MAX_PATH * 2] = {};

					if (ReadProcessMemory(
							hprocess,
							lpnameinhprocess,
							buffer,
							sizeof(buffer),
							NULL) &&
						MultiByteToWideChar(
							CP_ACP,
							0,
							buffer,
							-1,
							wbuffer,
							static_cast<int>(strlen(buffer))))
					{
						mappedFileName = std::wstring(wbuffer);
					}
				}
				else // We use UNICODE format
				{
					if (ReadProcessMemory(
						hprocess,
						lpnameinhprocess,
						wbuffer,
						sizeof(wbuffer),
						NULL))
					{
						mappedFileName = std::wstring(wbuffer);
					}
				}

				if (!mappedFileName.empty())
				{
					return mappedFileName;
				}
			}
		}

		// We can't bypass it
		// Calling default method to give the correct message
		return ComputeFilename(hfile);
	}

	//-------------------------------------------------------------------------
	std::map<std::wstring, LPVOID> HandleInformation::ComputeFilenames(
		HANDLE hprocess) const
	{
		std::map<std::wstring, LPVOID> modules = {};
		HMODULE hmodules[512];
		DWORD dwcounter = 0;
		TCHAR moduleName[MAX_PATH];
		MODULEINFO moduleinfo;

		ZeroMemory(&hmodules, 512 * sizeof(HMODULE));

		if (EnumProcessModules(
			hprocess,
			hmodules,
			sizeof(hmodules),
			&dwcounter))
		{
			size_t max = static_cast<size_t>(dwcounter) / sizeof(HMODULE);
			for (size_t index = 0; index < max; ++index)
			{
				ZeroMemory(&moduleName, sizeof(TCHAR) * MAX_PATH);
				ZeroMemory(&moduleinfo, sizeof(MODULEINFO));
				if (GetModuleFileNameEx(
						hprocess,
						hmodules[index],
						moduleName,
						sizeof(TCHAR) * MAX_PATH) &&
					GetModuleInformation(
						hprocess,
						hmodules[index],
						&moduleinfo,
						sizeof(MODULEINFO)))
				{
					modules.emplace(std::wstring(moduleName), moduleinfo.lpBaseOfDll);
				}
			}
		}

		return modules;
	}
}
