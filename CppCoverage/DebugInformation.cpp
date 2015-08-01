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

#include "stdafx.h"
#include "DebugInformation.hpp"

#define DBGHELP_TRANSLATE_TCHAR
#include <dbghelp.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>

#include "Tools/Tool.hpp"
#include "Tools/Log.hpp"
#include "Tools/ScopedAction.hpp"

#include "CppCoverageException.hpp"
#include "IDebugInformationEventHandler.hpp"
#include "Address.hpp"

namespace CppCoverage
{
	namespace
	{
		//---------------------------------------------------------------------
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

		//---------------------------------------------------------------------
		BOOL CALLBACK SymEnumLinesProc(PSRCCODEINFO lineInfo, PVOID userContext)
		{
			auto context = static_cast<Context*>(userContext);

			if (!context)
				THROW("Invalid user context.");
						
			DWORD64 addressValue = lineInfo->Address - lineInfo->ModBase + reinterpret_cast<DWORD64>(context->processBaseOfImage_);

			context->debugInformationEventHandler_.OnNewLine(
				lineInfo->FileName, lineInfo->LineNumber,
				Address{ context->hProcess_, reinterpret_cast<void*>(addressValue) });

			return TRUE;
		}		
				
		//---------------------------------------------------------------------
		BOOL CALLBACK SymEnumSourceFilesProc(PSOURCEFILE pSourceFile, PVOID userContext)			
		{
			auto context = static_cast<Context*>(userContext);

			if (!userContext)
				THROW("Invalid user context.");
			if (!pSourceFile)
				THROW("Source File is null");
			
			std::wstring filename = pSourceFile->FileName;
			
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

		//-------------------------------------------------------------------------
		BOOL CALLBACK SymRegisterCallbackProc64(
			_In_      HANDLE hProcess,
			_In_      ULONG actionCode,
			_In_opt_  ULONG64 callbackData,
			_In_opt_  ULONG64 userContext
			)
		{
			if (actionCode == CBA_DEBUG_INFO)
			{
				std::string message = reinterpret_cast<char*>(callbackData);
				
				boost::algorithm::replace_last(message, "\n", "");
				LOG_DEBUG << message;
				return TRUE;
			}

			return FALSE;
		}
	}
	
	//-------------------------------------------------------------------------
	DebugInformation::DebugInformation(HANDLE hProcess)
		: hProcess_(hProcess)
	{		
		SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES
				| SYMOPT_NO_UNQUALIFIED_LOADS | SYMOPT_UNDNAME | SYMOPT_DEBUG);
						
		if (!SymInitialize(hProcess_, nullptr, FALSE))
			THROW("Error when calling SymInitialize. You cannot call this function twice.");
		
		if (!SymRegisterCallback64(hProcess_, SymRegisterCallbackProc64, 0))
			THROW("Error when calling SymRegisterCallback64");

		wchar_t searchPath[8192];
		if (!SymGetSearchPath(hProcess_, searchPath, sizeof(searchPath) / sizeof(searchPath[0])))
			THROW_LAST_ERROR("Error when calling SymGetSearchPath", GetLastError());
		defaultSearchPath_ = searchPath;
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
		UpdateSearchPath(filename);
		auto baseAddress = SymLoadModuleEx(hProcess_, hFile, nullptr, nullptr, 0, 0, nullptr, 0);

		if (!baseAddress)
			THROW("Cannot load module for: " << filename);
		
		Tools::ScopedAction scopedAction{ [=]{ UnloadModule64(baseAddress);  } };

		Context context{ hProcess_, baseAddress, baseOfImage, debugInformationEventHandler };
			
		if (!SymEnumSourceFiles(hProcess_, baseAddress, nullptr, SymEnumSourceFilesProc, &context))
			LOG_WARNING << L"Cannot find pdb for " << filename;
	}

	//-------------------------------------------------------------------------
	void DebugInformation::UpdateSearchPath(const std::wstring& moduleFilename) const
	{
		auto parentPath = boost::filesystem::path(moduleFilename).parent_path();

		auto searchPath = defaultSearchPath_ + L';' + parentPath.wstring();
		if (!SymSetSearchPath(hProcess_, searchPath.c_str()))
			THROW_LAST_ERROR("Error when calling SymSetSearchPath", GetLastError());
	}

	//-------------------------------------------------------------------------
	void DebugInformation::UnloadModule64(DWORD64 baseOfDll) const
	{
		if (!SymUnloadModule64(hProcess_, baseOfDll))
			THROW("Cannot unload module");
	}

}
