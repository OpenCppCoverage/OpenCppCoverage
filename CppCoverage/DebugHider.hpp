#pragma once

#include <Windows.h>
#include <vector>

namespace CppCoverage
{
	///////////////////////////////////////////////////////////////////////////////////////
	//Evolution of Process Environment Block (PEB) http://blog.rewolf.pl/blog/?p=573
	//March 2, 2013 / ReWolf posted in programming, reverse engineering, source code, x64 /

	template <class T>
	struct LIST_ENTRY_T
	{
		T Flink;
		T Blink;
	};

	template <class T>
	struct UNICODE_STRING_T
	{
		union
		{
			struct
			{
				WORD Length;
				WORD MaximumLength;
			};
			T dummy;
		};
		T _Buffer;
	};

	template <class T, class NGF, int A>
	struct _PEB_T
	{
		union
		{
			struct
			{
				BYTE InheritedAddressSpace;
				BYTE ReadImageFileExecOptions;
				BYTE BeingDebugged;
				BYTE _SYSTEM_DEPENDENT_01;
			};
			T dummy01;
		};
		T Mutant;
		T ImageBaseAddress;
		T Ldr;
		T ProcessParameters;
		T SubSystemData;
		T ProcessHeap;
		T FastPebLock;
		T _SYSTEM_DEPENDENT_02;
		T _SYSTEM_DEPENDENT_03;
		T _SYSTEM_DEPENDENT_04;
		union
		{
			T KernelCallbackTable;
			T UserSharedInfoPtr;
		};
		DWORD SystemReserved;
		DWORD _SYSTEM_DEPENDENT_05;
		T _SYSTEM_DEPENDENT_06;
		T TlsExpansionCounter;
		T TlsBitmap;
		DWORD TlsBitmapBits[2];
		T ReadOnlySharedMemoryBase;
		T _SYSTEM_DEPENDENT_07;
		T ReadOnlyStaticServerData;
		T AnsiCodePageData;
		T OemCodePageData;
		T UnicodeCaseTableData;
		DWORD NumberOfProcessors;
		union
		{
			DWORD NtGlobalFlag;
			NGF dummy02;
		};
		LARGE_INTEGER CriticalSectionTimeout;
		T HeapSegmentReserve;
		T HeapSegmentCommit;
		T HeapDeCommitTotalFreeThreshold;
		T HeapDeCommitFreeBlockThreshold;
		DWORD NumberOfHeaps;
		DWORD MaximumNumberOfHeaps;
		T ProcessHeaps;
	};

	typedef _PEB_T<DWORD, DWORD64, 34> PEB32;
	typedef _PEB_T<DWORD64, DWORD, 30> PEB64;

#ifdef _WIN64
	typedef PEB64 PEB_CURRENT;
#else
	typedef PEB32 PEB_CURRENT;
#endif

	//Quote from The Ultimate Anti-Debugging Reference by Peter Ferrie
	//Flags field exists at offset 0x0C in the heap on the 32-bit versions of Windows NT, Windows 2000, and Windows XP; and at offset 0x40 on the 32-bit versions of Windows Vista and later.
	//Flags field exists at offset 0x14 in the heap on the 64-bit versions of Windows XP, and at offset 0x70 in the heap on the 64-bit versions of Windows Vista and later.
	//ForceFlags field exists at offset 0x10 in the heap on the 32-bit versions of Windows NT, Windows 2000, and Windows XP; and at offset 0x44 on the 32-bit versions of Windows Vista and later.
	//ForceFlags field exists at offset 0x18 in the heap on the 64-bit versions of Windows XP, and at offset 0x74 in the heap on the 64-bit versions of Windows Vista and later.

	static bool IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
	{
		OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
		DWORDLONG        const dwlConditionMask = VerSetConditionMask(
			VerSetConditionMask(
				VerSetConditionMask(
					0, VER_MAJORVERSION, VER_GREATER_EQUAL),
				VER_MINORVERSION, VER_GREATER_EQUAL),
			VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

		osvi.dwMajorVersion = wMajorVersion;
		osvi.dwMinorVersion = wMinorVersion;
		osvi.wServicePackMajor = wServicePackMajor;

		return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
	}

	static bool IsWindowsVistaOrGreater()
	{
		return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
	}

	static int getHeapFlagsOffset(bool x64)
	{
		if (x64)  //x64 offsets
		{
			if (IsWindowsVistaOrGreater())
			{
				return 0x70;
			}
			else
			{
				return 0x14;
			}
		}
		else //x86 offsets
		{
			if (IsWindowsVistaOrGreater())
			{
				return 0x40;
			}
			else
			{
				return 0x0C;
			}
		}
	}

	static int getHeapForceFlagsOffset(bool x64)
	{
		if (x64)  //x64 offsets
		{
			if (IsWindowsVistaOrGreater())
			{
				return 0x74;
			}
			else
			{
				return 0x18;
			}
		}
		else //x86 offsets
		{
			if (IsWindowsVistaOrGreater())
			{
				return 0x44;
			}
			else
			{
				return 0x10;
			}
		}
	}

	typedef struct _PROCESS_BASIC_INFORMATION
	{
		PVOID Reserved1;
		PVOID PebBaseAddress;
		PVOID Reserved2[2];
		ULONG_PTR UniqueProcessId;
		PVOID Reserved3;
	} PROCESS_BASIC_INFORMATION;

	typedef ULONG (NTAPI *p_NtQueryInformationProcess)(
		HANDLE /* ProcessHandle */,
		ULONG /* ProcessInformationClass */,
		PVOID /* ProcessInformation */,
		ULONG /* ProcessInformationLength */,
		PULONG /* ReturnLength */
		);

	static void* GetPEBLocation(HANDLE hProcess)
	{
		static auto NtQueryInformationProcess = (p_NtQueryInformationProcess)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationProcess");
		if (!NtQueryInformationProcess)
			return nullptr;

		ULONG RequiredLen = 0;
		void* PebAddress = nullptr;
		PROCESS_BASIC_INFORMATION myProcessBasicInformation[5] = { 0 };

		if (NtQueryInformationProcess(hProcess, 0 /* ProcessBasicInformation */, myProcessBasicInformation, sizeof(PROCESS_BASIC_INFORMATION), &RequiredLen) == 0)
		{
			PebAddress = (void*)myProcessBasicInformation->PebBaseAddress;
		}
		else
		{
			if (NtQueryInformationProcess(hProcess, 0 /* ProcessBasicInformation */, myProcessBasicInformation, RequiredLen, &RequiredLen) == 0)
			{
				PebAddress = (void*)myProcessBasicInformation->PebBaseAddress;
			}
		}

		return PebAddress;
	}

	static bool PebPatchHeapFlags(PEB_CURRENT* peb, HANDLE hProcess)
	{
#ifdef _WIN64
		const auto is_x64 = true;
#else
		const auto is_x64 = false;
#endif

		std::vector<PVOID> heaps;
		heaps.resize(peb->NumberOfHeaps);

		if (ReadProcessMemory(hProcess, (PVOID)peb->ProcessHeaps, (PVOID)heaps.data(), heaps.size() * sizeof(PVOID), nullptr) == FALSE)
			return false;

		std::basic_string<uint8_t> heap;
		heap.resize(0x100); // hacky
		for (DWORD i = 0; i < peb->NumberOfHeaps; i++)
		{
			if (ReadProcessMemory(hProcess, heaps[i], (PVOID)heap.data(), heap.size(), nullptr) == FALSE)
				return false;

			auto flags = (DWORD *)(heap.data() + getHeapFlagsOffset(is_x64));
			auto force_flags = (DWORD *)(heap.data() + getHeapForceFlagsOffset(is_x64));

			if (i == 0)
			{
				// Default heap.
				*flags &= HEAP_GROWABLE;
			}
			else
			{
				// Flags from RtlCreateHeap/HeapCreate.
				*flags &= (HEAP_GROWABLE | HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE);
			}

			*force_flags = 0;

			if (WriteProcessMemory(hProcess, heaps[i], (PVOID)heap.data(), heap.size(), nullptr) == FALSE)
				return false;
		}

		return true;
	}

	static bool HidePebInProcess(HANDLE hProcess)
	{
		PEB_CURRENT myPEB = { 0 };
		SIZE_T ueNumberOfBytesRead = 0;
		void* heapFlagsAddress = 0;
		DWORD heapFlags = 0;
		void* heapForceFlagsAddress = 0;
		DWORD heapForceFlags = 0;

		void* AddressOfPEB = GetPEBLocation(hProcess);

		if (!AddressOfPEB)
			return false;

		if (ReadProcessMemory(hProcess, AddressOfPEB, (void*)&myPEB, sizeof(PEB_CURRENT), &ueNumberOfBytesRead))
		{
			myPEB.BeingDebugged = FALSE;
			myPEB.NtGlobalFlag &= ~0x70;

#ifdef _WIN64
			heapFlagsAddress = (void*)((LONG_PTR)myPEB.ProcessHeap + getHeapFlagsOffset(true));
			heapForceFlagsAddress = (void*)((LONG_PTR)myPEB.ProcessHeap + getHeapForceFlagsOffset(true));
#else
			heapFlagsAddress = (void*)((LONG_PTR)myPEB.ProcessHeap + getHeapFlagsOffset(false));
			heapForceFlagsAddress = (void*)((LONG_PTR)myPEB.ProcessHeap + getHeapForceFlagsOffset(false));
#endif //_WIN64

			ReadProcessMemory(hProcess, heapFlagsAddress, &heapFlags, sizeof(DWORD), 0);
			ReadProcessMemory(hProcess, heapForceFlagsAddress, &heapForceFlags, sizeof(DWORD), 0);

			heapFlags &= HEAP_GROWABLE;
			heapForceFlags = 0;

			WriteProcessMemory(hProcess, heapFlagsAddress, &heapFlags, sizeof(DWORD), 0);
			WriteProcessMemory(hProcess, heapForceFlagsAddress, &heapForceFlags, sizeof(DWORD), 0);

			PebPatchHeapFlags(&myPEB, hProcess);

			if (WriteProcessMemory(hProcess, AddressOfPEB, (void*)&myPEB, sizeof(PEB_CURRENT), &ueNumberOfBytesRead))
			{
				return true;
			}
		}
		return false;
	}
}