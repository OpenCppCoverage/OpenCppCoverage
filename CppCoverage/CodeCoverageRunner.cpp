#include "stdafx.h"
#include "CodeCoverageRunner.hpp"

#include <boost/optional.hpp>

#include "CoverageData.hpp"
#include "Debugger.hpp"
#include "DebugInformation.hpp"
#include "ExecutedAddressManager.hpp"
#include "HandleInformation.hpp"
#include "BreakPoint.hpp"
#include "CoverageFilter.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	CodeCoverageRunner::CodeCoverageRunner()
	{ 
		executedAddressManager_.reset(new ExecutedAddressManager());
	}

	//-------------------------------------------------------------------------
	CoverageData CodeCoverageRunner::RunCoverage(const StartInfo& startInfo, const CoverageSettings&)
	{
		CoverageData test;		
		Debugger debugger;

		coverageFilter_.reset(new CoverageFilter());
		debugger.Debug(startInfo, *this);
	
		return test;
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO& processDebugInfo)
	{
		auto hProcess = processDebugInfo.hProcess;

		debugInformation_.reset(new DebugInformation(hProcess));
		breakpoint_.reset(new BreakPoint(hProcess));
		LoadModule(processDebugInfo.hFile);
	}
	
	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnLoadDll(const LOAD_DLL_DEBUG_INFO& dllDebugInfo)
	{
		LoadModule(dllDebugInfo.hFile);
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnException(const EXCEPTION_DEBUG_INFO& exceptionDebugInfo)
	{
		if (exceptionDebugInfo.dwFirstChance)
		{
			const auto& exceptionRecord = exceptionDebugInfo.ExceptionRecord;
			auto address = exceptionRecord.ExceptionAddress;

			auto oldInstruction = executedAddressManager_->MarkAddressAsExecuted(address);

			breakpoint_->RemoveBreakPoint(address, oldInstruction);
		}		
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::LoadModule(HANDLE hFile)
	{
		HandleInformation handleInformation;

		std::wstring filename = handleInformation.ComputeFilename(hFile);

		if (coverageFilter_->IsModuleSelected(filename))
		{
			executedAddressManager_->SetCurrentModule(filename);
			debugInformation_->LoadModule(hFile, *this);
		}
	}

	//-------------------------------------------------------------------------
	bool CodeCoverageRunner::IsSourceFileSelected(const std::wstring& filename) const
	{
		return coverageFilter_->IsSourceFileSelected(filename);
	}
	
	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnNewLine(
		const std::wstring& filename, 
		int lineNumber, 
		DWORD64 address)
	{
		auto addressPtr = reinterpret_cast<void*>(address);
		auto oldInstruction = breakpoint_->SetBreakPointAt(addressPtr);

		executedAddressManager_->RegisterAddress(addressPtr, filename, lineNumber, oldInstruction);
	}
}
