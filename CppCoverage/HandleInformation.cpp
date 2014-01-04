#include "stdafx.h"
#include "HandleInformation.hpp"

#include <Psapi.h>
#include <boost/algorithm/string.hpp>

#include "CppCoverageException.hpp"


#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <strsafe.h> // $$

namespace CppCoverage
{
	namespace
	{

		template<typename T_Handle, typename T_Releaser>
		class Handle // $$$ move to file
		{
		public:
			explicit Handle(T_Handle handle, T_Releaser releaser)
				: handle_(handle)
				, releaser_(releaser)
			{
				if (!handle_)
					THROW(L"Handle is not valid");
			}

			Handle(Handle&& handle)
				: handle_()
				, releaser_()
			{
				std::swap(handle_, handle.handle_);
				std::swap(releaser_, handle.releaser_);
			}

			T_Handle GetValue() const
			{
				return handle_;
			}

			~Handle()
			{
				try
				{
					if (handle_ && !releaser_(handle_))
					{
						// $$ Log ERROR
					}
				}
				catch (const std::exception&) // $$ factorize this code
				{
					// log $$$
				}
				catch (...)
				{
				}
			}

		private:
			Handle(const Handle&) = delete;
			Handle& operator=(const Handle&) = delete;

		private:
			T_Handle handle_;
			T_Releaser releaser_;
		};

		template<typename T_Handle, typename T_Releaser>
		Handle<T_Handle, T_Releaser> CreateHandle(T_Handle handle, T_Releaser releaser)
		{
			return Handle<T_Handle, T_Releaser>(handle, releaser);
		}

		//-------------------------------------------------------------------------
		std::wstring GetMappedFileNameStr(HANDLE hfile)
		{
			DWORD dwFileSizeHi = 0;
			DWORD dwFileSizeLo = GetFileSize(hfile, &dwFileSizeHi);

			if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
				THROW(L"Cannot map a file with a length of zero.");
			
			HANDLE hFileMap = CreateFileMapping(hfile, NULL, PAGE_READONLY, 0, 1, NULL);
			auto fileMappingHandle = CreateHandle(hFileMap, CloseHandle);

			auto mapHandle = MapViewOfFile(fileMappingHandle.GetValue(), FILE_MAP_READ, 0, 0, 1);
			auto mapViewOfFile = CreateHandle(mapHandle, UnmapViewOfFile);

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
		std::vector<std::wstring> GetLogicalDrives()
		{
			wchar_t logicalDriveStrings[4096];
			logicalDriveStrings[0] = '\0';
			std::vector<std::wstring> logicalDrives;

			auto size = GetLogicalDriveStrings(MAX_PATH - 1, logicalDriveStrings);

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
	}

	std::pair<std::wstring, std::wstring> GetMatchingDriveInfo(
		const std::wstring& mappedFileNameStr,
		const std::vector<std::wstring>& logicalDrives)
	{		
		wchar_t dosDevice[MAX_PATH];

		for (const auto& logicalDrive : logicalDrives)
		{
			auto pos = logicalDrive.find('\\');
			std::wstring drive = (pos != std::string::npos) ? logicalDrive.substr(0, pos) : logicalDrive;

			if (QueryDosDevice(drive.c_str(), dosDevice, sizeof(dosDevice)))
			{
				if (mappedFileNameStr.find(dosDevice) == 0)
					return std::make_pair(drive, dosDevice);
			}
		}

		THROW(L"Cannot find drive for " << mappedFileNameStr);
	}

	//-------------------------------------------------------------------------
	std::wstring HandleInformation::ComputeFilename(HANDLE hfile) const
	{
		std::wstring mappedFileNameStr = GetMappedFileNameStr(hfile);
		auto logicalDrives = GetLogicalDrives();
		
		auto matchingDrive = GetMatchingDriveInfo(mappedFileNameStr, logicalDrives);

		boost::algorithm::replace_first(mappedFileNameStr, matchingDrive.second, matchingDrive.first);
	
		return mappedFileNameStr;
	}
}
