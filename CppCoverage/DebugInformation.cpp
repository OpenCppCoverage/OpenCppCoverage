#include "stdafx.h"
#include "DebugInformation.hpp"

#include <dbghelp.h>
#include "Tools/Tool.hpp"
#include "Tools/Log.hpp"
#include "Tools/ScopedAction.hpp"

#include "CppCoverageException.hpp"
#include "IDebugInformationEventHandler.hpp"

namespace CppCoverage
{
	namespace
	{
		struct Context
		{
			Context(
				HANDLE hProcess, 
				DWORD64 baseAddress, 
				void* processBaseOfImage,
				IDebugInformationEventHandler& debugInformationEventHandler)
				: hProcess_(hProcess)
				, baseAddress_(baseAddress)
				, processBaseOfImage_(processBaseOfImage)
				, debugInformationEventHandler_(debugInformationEventHandler)
			{
			}

			HANDLE hProcess_;
			DWORD64 baseAddress_;
			void* processBaseOfImage_;
			IDebugInformationEventHandler& debugInformationEventHandler_;
		};

		BOOL CALLBACK SymEnumLinesProc(PSRCCODEINFO lineInfo, PVOID userContext)
		{
			auto context = static_cast<Context*>(userContext);

			if (!userContext)
				THROW("Invalid user context.");
			
			DWORD64 address = lineInfo->Address - lineInfo->ModBase + reinterpret_cast<DWORD64>(context->processBaseOfImage_);
			std::wstring filename = Tools::Tool::ToWString(lineInfo->FileName);
			context->debugInformationEventHandler_.OnNewLine(filename, lineInfo->LineNumber, address);

			return TRUE;
		}		

		BOOL CALLBACK SymEnumSourceFilesProc(PSOURCEFILE pSourceFile, PVOID userContext)			
		{
			auto context = static_cast<Context*>(userContext);

			if (!userContext)
				THROW("Invalid user context.");
			if (!pSourceFile)
				THROW("Source File is null");
			
			std::wstring filename = Tools::Tool::ToWString(pSourceFile->FileName);
			
			if (context->debugInformationEventHandler_.IsSourceFileSelected(filename))
			{				
				if (!SymEnumSourceLines(
					context->hProcess_,
					context->baseAddress_,
					nullptr,
					pSourceFile->FileName,
					0,
					ESLFLAG_FULLPATH,
					SymEnumLinesProc,
					userContext))
				{
					THROW("Cannot enumerate source lines for" << pSourceFile->FileName);
				}
			}

			return TRUE;
		}
	}

	//-------------------------------------------------------------------------
	DebugInformation::DebugInformation(HANDLE hProcess)
		: hProcess_(hProcess)
	{		
		SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES 
			        | SYMOPT_NO_UNQUALIFIED_LOADS | SYMOPT_UNDNAME);

		// $$ test if not found pdb		
		if (!SymInitialize(hProcess_, nullptr, FALSE))
			THROW("Error when calling SymInitialize. You cannot call this function twice.");
	}

	//-------------------------------------------------------------------------
	DebugInformation::~DebugInformation()
	{
		if (!SymCleanup(hProcess_))
		{
			LOG_ERROR << "Error in SymCleanup";
		}
	}
	
	//-------------------------------------------------------------------------
	void DebugInformation::LoadModule(
		const std::wstring& filename,
		HANDLE hFile,
		void* baseOfImage,
		IDebugInformationEventHandler& debugInformationEventHandler) const
	{		
		auto baseAddress = SymLoadModuleEx(hProcess_, hFile, nullptr, nullptr, 0, 0, nullptr, 0);

		if (!baseAddress)
			THROW("Cannot load module for: " << filename);
		
		Tools::ScopedAction scopedAction{ [=]{ UnloadModule64(baseAddress);  } };

		Context context{ hProcess_, baseAddress, baseOfImage, debugInformationEventHandler };
			
		if (!SymEnumSourceFiles(hProcess_, baseAddress, nullptr, SymEnumSourceFilesProc, &context))
			LOG_WARNING << "Cannot find pdb for " << filename;
	}

	//-------------------------------------------------------------------------
	void DebugInformation::UnloadModule64(DWORD64 baseOfDll) const
	{
		if (!SymUnloadModule64(hProcess_, baseOfDll))
			THROW("Cannot unload module");
	}

}
