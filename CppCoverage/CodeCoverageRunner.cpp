#include "stdafx.h"
#include "CodeCoverageRunner.hpp"

#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include "CoverageData.hpp"
#include "Debugger.hpp"
#include "DebugInformation.hpp"
#include "ExecutedAddressManager.hpp"
#include "HandleInformation.hpp"
#include "BreakPoint.hpp"
#include "CoverageFilter.hpp"
#include "StartInfo.hpp"

namespace CppCoverage
{
	// $$$ 
	// http://stackoverflow.com/questions/11652519/generate-xml-using-xerces-c
	// http://code.google.com/p/google-code-prettify/
	// http://shjs.sourceforge.net/
	// http://www.gnu.org/software/src-highlite/
	//-------------------------------------------------------------------------
	CodeCoverageRunner::CodeCoverageRunner()
	{ 
		executedAddressManager_.reset(new ExecutedAddressManager());
	}

	//-------------------------------------------------------------------------
	CodeCoverageRunner::~CodeCoverageRunner()
	{
	}

	//-------------------------------------------------------------------------
	CoverageData CodeCoverageRunner::RunCoverage(const StartInfo& startInfo, const CoverageSettings& coverageSettings)
	{
		Debugger debugger;

		coverageFilter_.reset(new CoverageFilter(coverageSettings));
		debugger.Debug(startInfo, *this);

		boost::filesystem::path path{startInfo.GetFilename()};

		return executedAddressManager_->CreateCoverageData(path.filename().wstring());
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO& processDebugInfo)
	{
		auto hProcess = processDebugInfo.hProcess;
		auto lpBaseOfImage = processDebugInfo.lpBaseOfImage;

		debugInformation_.reset(new DebugInformation(hProcess));
		breakpoint_.reset(new BreakPoint(hProcess));
		LoadModule(processDebugInfo.hFile, lpBaseOfImage);
		isFirstException_ = true;
	}
	
	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnLoadDll(
		HANDLE hProcess, 
		HANDLE hThread, 
		const LOAD_DLL_DEBUG_INFO& dllDebugInfo)
	{
		LoadModule(dllDebugInfo.hFile, dllDebugInfo.lpBaseOfDll);
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::OnException(
		HANDLE hProcess, 
		HANDLE hThread, 
		const EXCEPTION_DEBUG_INFO& exceptionDebugInfo)
	{
		const auto& exceptionRecord = exceptionDebugInfo.ExceptionRecord;						
		
		if (
			   exceptionDebugInfo.dwFirstChance
			&& exceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT
			&& !isFirstException_) // Skip the first exception set by default.
		{
			const auto& exceptionRecord = exceptionDebugInfo.ExceptionRecord;
			auto address = exceptionRecord.ExceptionAddress;

			auto oldInstruction = executedAddressManager_->MarkAddressAsExecuted(address);

			breakpoint_->RemoveBreakPoint(address, oldInstruction);			
			breakpoint_->AdjustEipAfterBreakPointRemoval(hThread);
		}			
		isFirstException_ = false;
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::LoadModule(HANDLE hFile, void* baseOfImage)
	{
		HandleInformation handleInformation;

		std::wstring filename = handleInformation.ComputeFilename(hFile);

		if (coverageFilter_->IsModuleSelected(filename))
		{
			executedAddressManager_->SetCurrentModule(filename);
			debugInformation_->LoadModule(hFile, baseOfImage, *this);
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
