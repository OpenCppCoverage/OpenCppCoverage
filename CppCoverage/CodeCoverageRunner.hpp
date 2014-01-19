#ifndef CPPCOVERAGE_CODECOVERAGERUNNER_HEADER_GARD
#define CPPCOVERAGE_CODECOVERAGERUNNER_HEADER_GARD

#include <memory>

#include "CoverageData.hpp"
#include "IDebugEventsHandler.hpp"
#include "IDebugInformationEventHandler.hpp"
#include "Export.hpp"

namespace CppCoverage
{
	class StartInfo;
	class CoverageSettings;
	class DebugInformation;
	class DebugInformationEventHandler;
	class ExecutedAddressManager;
	class BreakPoint;
	class CoverageFilter;

	class DLL CodeCoverageRunner : private IDebugEventsHandler, private IDebugInformationEventHandler
	{
	public:
		CodeCoverageRunner();
		~CodeCoverageRunner();

		CoverageData RunCoverage(const StartInfo&, const CoverageSettings&);

	private:
		virtual void OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO&) override;
		virtual void OnLoadDll(HANDLE hProcess, HANDLE hThread, const LOAD_DLL_DEBUG_INFO&) override;
		virtual void OnException(HANDLE hProcess, HANDLE hThread, const EXCEPTION_DEBUG_INFO&) override;

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
		bool isFirstException_;
	};
}

#endif
