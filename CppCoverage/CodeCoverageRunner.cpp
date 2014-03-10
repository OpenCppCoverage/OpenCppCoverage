#include "stdafx.h"
#include "CodeCoverageRunner.hpp"

#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include "tools/Log.hpp"

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

	const std::string CodeCoverageRunner::unhandledExceptionErrorMessage = "Unhandled exception: ";

	// $$$ Manage file collision  (same file in different folder or module)
	//-------------------------------------------------------------------------
	CodeCoverageRunner::CodeCoverageRunner()
	{ 
		InitExceptionCode();
		executedAddressManager_.reset(new ExecutedAddressManager());
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::InitExceptionCode()
	{
		exceptionCode_.emplace(EXCEPTION_ACCESS_VIOLATION, L"EXCEPTION_ACCESS_VIOLATION");
		exceptionCode_.emplace(EXCEPTION_ARRAY_BOUNDS_EXCEEDED, L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
		exceptionCode_.emplace(EXCEPTION_BREAKPOINT, L"EXCEPTION_BREAKPOINT");
		exceptionCode_.emplace(EXCEPTION_DATATYPE_MISALIGNMENT, L"EXCEPTION_DATATYPE_MISALIGNMENT");
		exceptionCode_.emplace(EXCEPTION_FLT_DENORMAL_OPERAND, L"EXCEPTION_FLT_DENORMAL_OPERAND");
		exceptionCode_.emplace(EXCEPTION_FLT_DIVIDE_BY_ZERO, L"EXCEPTION_FLT_DIVIDE_BY_ZERO");
		exceptionCode_.emplace(EXCEPTION_FLT_INEXACT_RESULT, L"EXCEPTION_FLT_INEXACT_RESULT");
		exceptionCode_.emplace(EXCEPTION_FLT_INVALID_OPERATION, L"EXCEPTION_FLT_INVALID_OPERATION");
		exceptionCode_.emplace(EXCEPTION_FLT_OVERFLOW, L"EXCEPTION_FLT_OVERFLOW");
		exceptionCode_.emplace(EXCEPTION_FLT_STACK_CHECK, L"EXCEPTION_FLT_STACK_CHECK");
		exceptionCode_.emplace(EXCEPTION_FLT_UNDERFLOW, L"EXCEPTION_FLT_UNDERFLOW");
		exceptionCode_.emplace(EXCEPTION_ILLEGAL_INSTRUCTION, L"EXCEPTION_ILLEGAL_INSTRUCTION");
		exceptionCode_.emplace(EXCEPTION_IN_PAGE_ERROR, L"EXCEPTION_IN_PAGE_ERROR");
		exceptionCode_.emplace(EXCEPTION_INT_DIVIDE_BY_ZERO, L"EXCEPTION_INT_DIVIDE_BY_ZERO");
		exceptionCode_.emplace(EXCEPTION_INT_OVERFLOW, L"EXCEPTION_INT_OVERFLOW");
		exceptionCode_.emplace(EXCEPTION_INVALID_DISPOSITION, L"EXCEPTION_INVALID_DISPOSITION");
		exceptionCode_.emplace(EXCEPTION_NONCONTINUABLE_EXCEPTION, L"EXCEPTION_NONCONTINUABLE_EXCEPTION");
		exceptionCode_.emplace(EXCEPTION_PRIV_INSTRUCTION, L"EXCEPTION_PRIV_INSTRUCTION");
		exceptionCode_.emplace(EXCEPTION_SINGLE_STEP, L"EXCEPTION_SINGLE_STEP");
		exceptionCode_.emplace(EXCEPTION_STACK_OVERFLOW, L"EXCEPTION_STACK_OVERFLOW");
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
	DWORD CodeCoverageRunner::OnException(
		HANDLE hProcess, 
		HANDLE hThread, 
		const EXCEPTION_DEBUG_INFO& exceptionDebugInfo)
	{
		const auto& exceptionRecord = exceptionDebugInfo.ExceptionRecord;								
		auto address = exceptionRecord.ExceptionAddress;

		if (exceptionDebugInfo.dwFirstChance)
		{
			if (!isFirstException_ && exceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT) // Skip the first exception set by default.
			{
				auto oldInstruction = executedAddressManager_->MarkAddressAsExecuted(address);

				breakpoint_->RemoveBreakPoint(address, oldInstruction);
				breakpoint_->AdjustEipAfterBreakPointRemoval(hThread);
				return DBG_CONTINUE;
			}
		}
		else
			LOG_ERROR << unhandledExceptionErrorMessage << GetExceptionStrFromCode(exceptionRecord.ExceptionCode);
	
		isFirstException_ = false;
		return DBG_EXCEPTION_NOT_HANDLED;
	}

	//-------------------------------------------------------------------------
	std::wstring CodeCoverageRunner::GetExceptionStrFromCode(DWORD exceptionCode) const
	{
		auto it = exceptionCode_.find(exceptionCode);

		if (it != exceptionCode_.end())
			return it->second;
		return L"Unknown";
	}

	//-------------------------------------------------------------------------
	void CodeCoverageRunner::LoadModule(HANDLE hFile, void* baseOfImage)
	{
		HandleInformation handleInformation;

		std::wstring filename = handleInformation.ComputeFilename(hFile);
		
		if (coverageFilter_->IsModuleSelected(filename))
		{
			executedAddressManager_->SetCurrentModule(filename);
			debugInformation_->LoadModule(filename, hFile, baseOfImage, *this);
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

		if (!executedAddressManager_->RegisterAddress(addressPtr, filename, lineNumber, oldInstruction))
			breakpoint_->RemoveBreakPoint(addressPtr, oldInstruction);
	}
}
