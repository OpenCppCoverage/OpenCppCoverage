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

#pragma once

#include <memory>

#include "Plugin/Exporter/CoverageData.hpp"
#include "IDebugEventsHandler.hpp"
#include "CppCoverageExport.hpp"

namespace Tools
{
	class WarningManager;
}

namespace CppCoverage
{
	class StartInfo;
	class RunCoverageSettings;
	class DebugInformationEventHandler;
	class ExecutedAddressManager;
	class BreakPoint;
	class CoverageFilterManager;
	class ExceptionHandler;
	class UnifiedDiffSettings;
	class MonitoredLineRegister;
	class FilterAssistant;

	class CPPCOVERAGE_DLL CodeCoverageRunner : private IDebugEventsHandler
	{
	public:
		explicit CodeCoverageRunner(std::shared_ptr<Tools::WarningManager>);
		~CodeCoverageRunner();

		Plugin::CoverageData RunCoverage(const RunCoverageSettings&);

	private:
		virtual void OnCreateProcess(HANDLE hProces, const wchar_t* pszImageName, void* lpBaseOfImage) override;
		virtual void OnExitProcess(HANDLE hProcess) override;
		virtual void OnLoadDll(HANDLE hProcess, const wchar_t* pszImageName, void* lpBaseOfImage) override;
		virtual void OnUnloadDll(HANDLE hProcess, void* lpBaseOfImage) override;
		virtual ExceptionType OnException(HANDLE hProcess, HANDLE hThread, const EXCEPTION_DEBUG_INFO&) override;

	private:
		CodeCoverageRunner(const CodeCoverageRunner&) = delete;
		CodeCoverageRunner& operator=(const CodeCoverageRunner&) = delete;

		void LoadModule(HANDLE hProcess, const wchar_t* pszImageName, void* baseOfImage);
		bool OnBreakPoint(const EXCEPTION_DEBUG_INFO&, HANDLE hProcess, HANDLE hThread);

	private:
		std::shared_ptr<BreakPoint> breakpoint_;
		std::shared_ptr<ExecutedAddressManager> executedAddressManager_;
		std::shared_ptr<CoverageFilterManager> coverageFilterManager_;
		std::unique_ptr<MonitoredLineRegister> monitoredLineRegister_;
		std::unique_ptr<ExceptionHandler> exceptionHandler_;
		std::shared_ptr<Tools::WarningManager> warningManager_;
		std::shared_ptr<FilterAssistant> filterAssistant_;
	};
}


