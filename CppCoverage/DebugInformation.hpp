#pragma once

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class IDebugInformationEventHandler;

	class CPPCOVERAGE_DLL DebugInformation
	{
	public:
		explicit DebugInformation(HANDLE hProcess);
		~DebugInformation();

		void LoadModule(
			const std::wstring& filename, 
			HANDLE hFile, 
			void* baseOfImage, 
			IDebugInformationEventHandler& debugInformationEventHandler) const;

	private:
		DebugInformation(const DebugInformation&) = delete;
		DebugInformation& operator=(const DebugInformation&) = delete;

		void UnloadModule64(DWORD64 baseOfDll) const;

	private:
		HANDLE hProcess_;
	};
}


