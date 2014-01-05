#ifndef CPPCOVERAGE_CODECOVERAGERUNNER_HEADER_GARD
#define CPPCOVERAGE_CODECOVERAGERUNNER_HEADER_GARD

#include <memory>

#include "CoverageData.hpp"
#include "IDebugEventsHandler.hpp"
#include "IDebugInformationEventHandler.hpp"

namespace CppCoverage
{
	class StartInfo;
	class CoverageSettings;
	class DebugInformation;
	class DebugInformationEventHandler;
	class ExecutedAddressManager;
	class BreakPoint;
	class CoverageFilter;

	class CodeCoverageRunner : private IDebugEventsHandler, private IDebugInformationEventHandler
	{
	public:
		CodeCoverageRunner();

		CoverageData RunCoverage(const StartInfo&, const CoverageSettings&);

	private:
		virtual void OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO&);
		virtual void OnLoadDll(const LOAD_DLL_DEBUG_INFO&);
		virtual void OnException(const EXCEPTION_DEBUG_INFO&);		

	private:
		virtual bool IsSourceFileSelected(const std::wstring& filename) const;
		virtual void OnNewLine(const std::wstring& fileName, int lineNumber, DWORD64 address);

	private:
		CodeCoverageRunner(const CodeCoverageRunner&) = delete;
		CodeCoverageRunner& operator=(const CodeCoverageRunner&) = delete;

		void LoadModule(HANDLE hFile);

	private:
		std::unique_ptr<DebugInformation> debugInformation_;		
		std::unique_ptr<BreakPoint> breakpoint_;
		std::unique_ptr<ExecutedAddressManager> executedAddressManager_;
		std::unique_ptr<CoverageFilter> coverageFilter_;
	};
}

#endif
