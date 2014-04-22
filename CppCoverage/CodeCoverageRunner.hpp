#pragma once

#include <memory>
#include <unordered_map>

#include "CoverageData.hpp"
#include "IDebugEventsHandler.hpp"
#include "IDebugInformationEventHandler.hpp"
#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class StartInfo;
	class CoverageSettings;
	class DebugInformation;
	class DebugInformationEventHandler;
	class ExecutedAddressManager;
	class BreakPoint;
	class CoverageFilter;

	class CPPCOVERAGE_DLL CodeCoverageRunner : private IDebugEventsHandler, private IDebugInformationEventHandler
	{
	public:
		static const std::string unhandledExceptionErrorMessage;

	public:
		CodeCoverageRunner();
		~CodeCoverageRunner();

		CoverageData RunCoverage(const StartInfo&, const CoverageSettings&);

	private:
		virtual void OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO&) override;
		virtual void OnLoadDll(HANDLE hProcess, HANDLE hThread, const LOAD_DLL_DEBUG_INFO&) override;
		virtual DWORD OnException(HANDLE hProcess, HANDLE hThread, const EXCEPTION_DEBUG_INFO&) override;

	private:
		virtual bool IsSourceFileSelected(const std::wstring& filename) const override;
		virtual void OnNewLine(const std::wstring& fileName, int lineNumber, DWORD64 address) override;

	private:
		CodeCoverageRunner(const CodeCoverageRunner&) = delete;
		CodeCoverageRunner& operator=(const CodeCoverageRunner&) = delete;

		void LoadModule(HANDLE hFile, void* baseOfImage);
		void InitExceptionCode();
		std::wstring GetExceptionStrFromCode(DWORD) const;

	private:
		std::unique_ptr<DebugInformation> debugInformation_;		
		std::unique_ptr<BreakPoint> breakpoint_;
		std::unique_ptr<ExecutedAddressManager> executedAddressManager_;
		std::unique_ptr<CoverageFilter> coverageFilter_;
		std::unordered_map<DWORD, std::wstring> exceptionCode_;
		bool isFirstException_;
	};
}


