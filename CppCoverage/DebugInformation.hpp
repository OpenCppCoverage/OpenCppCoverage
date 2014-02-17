#ifndef CPPCOVERAGE_DEBUGINFORMATION_HEADER_GARD
#define CPPCOVERAGE_DEBUGINFORMATION_HEADER_GARD

#include "Export.hpp"

namespace CppCoverage
{
	class IDebugInformationEventHandler;

	class CPPCOVERAGE_DLL DebugInformation
	{
	public:
		explicit DebugInformation(HANDLE hProcess);
		~DebugInformation();

		void LoadModule(HANDLE hFile, void* baseOfImage, IDebugInformationEventHandler& debugInformationEventHandler) const;

	private:
		DebugInformation(const DebugInformation&) = delete;
		DebugInformation& operator=(const DebugInformation&) = delete;

		void UnloadModule64(DWORD64 baseOfDll) const;

	private:
		HANDLE hProcess_;
	};
}

#endif
