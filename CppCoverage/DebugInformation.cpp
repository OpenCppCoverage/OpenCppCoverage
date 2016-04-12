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

#include <boost/algorithm/string.hpp>
#include <boost/optional/optional.hpp>

#include "Tools/DbgHelp.hpp"
#include "Tools/Tool.hpp"
#include "Tools/Log.hpp"
#include "Tools/ScopedAction.hpp"

#include "CppCoverageException.hpp"
#include "ICoverageFilterManager.hpp"

namespace CppCoverage
{
	namespace
	{
		//---------------------------------------------------------------------
		struct Context
		{
			Context(
				DWORD64 baseAddress, 
				void* processBaseOfImage,
				const FileDebugInformation& fileDebugInformation,
				ICoverageFilterManager& coverageFilterManager,
				IDebugInformationEventHandler& debugInformationEventHandler)
				: baseAddress_(baseAddress)
				, processBaseOfImage_(processBaseOfImage)
				, fileDebugInformation_{ fileDebugInformation }
				, coverageFilterManager_(coverageFilterManager)
				, debugInformationEventHandler_(debugInformationEventHandler)
			{
			}

			DWORD64 baseAddress_;
			void* processBaseOfImage_;
			const FileDebugInformation& fileDebugInformation_;
			IDebugInformationEventHandler& debugInformationEventHandler_;
			ICoverageFilterManager& coverageFilterManager_;
			boost::optional<std::wstring> error_;
		};

		//---------------------------------------------------------------------
		BOOL CALLBACK SymEnumSourceFilesProc(PSOURCEFILE pSourceFile, PVOID userContext)			
		{
			auto context = static_cast<Context*>(userContext);

			if (!userContext)
			{
				LOG_ERROR << L"Invalid user context.";
				return FALSE;
			}
			
			context->error_ = Tools::Try([&]()
			{
				if (!pSourceFile)
					THROW("Source File is null");

				auto filename = Tools::ToWString(pSourceFile->FileName);

				if (context->coverageFilterManager_.IsSourceFileSelected(filename))
				{
					context->fileDebugInformation_.LoadFile(
						context->processBaseOfImage_,
						context->baseAddress_,
						filename,
						context->coverageFilterManager_,
						context->debugInformationEventHandler_);
				}
			});
			return context->error_ ? FALSE : TRUE;
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
		, fileDebugInformation_{hProcess}
	{		
		SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES
				| SYMOPT_NO_UNQUALIFIED_LOADS | SYMOPT_UNDNAME | SYMOPT_DEBUG);
						
		if (!SymInitialize(hProcess_, nullptr, FALSE))
			THROW("Error when calling SymInitialize. You cannot call this function twice.");
		
		if (!SymRegisterCallback64(hProcess_, SymRegisterCallbackProc64, 0))
			THROW("Error when calling SymRegisterCallback64");

		std::vector<char> searchPath(PathBufferSize);
		if (!SymGetSearchPath(hProcess_, &searchPath[0], static_cast<int>(searchPath.size())))
			THROW_LAST_ERROR("Error when calling SymGetSearchPath", GetLastError());
		defaultSearchPath_ = &searchPath[0];
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
		ICoverageFilterManager& coverageFilterManager,
		IDebugInformationEventHandler& debugInformationEventHandler) const
	{		
		UpdateSearchPath(filename);
		auto baseAddress = SymLoadModuleEx(hProcess_, hFile, nullptr, nullptr, 0, 0, nullptr, 0);

		if (!baseAddress)
			THROW("Cannot load module for: " << filename);

		Tools::ScopedAction scopedAction{ [=] {
			if (!SymUnloadModule64(hProcess_, baseAddress))
				THROW("UnloadModule64 ");
		} };

		Context context{ baseAddress, baseOfImage, fileDebugInformation_, coverageFilterManager, debugInformationEventHandler };

		if (!SymEnumSourceFiles(hProcess_, baseAddress, nullptr, SymEnumSourceFilesProc, &context))
			LOG_WARNING << L"Cannot find pdb for " << filename;
		if (context.error_)
			throw std::runtime_error(Tools::ToString(*context.error_));
	}

	//-------------------------------------------------------------------------
	void DebugInformation::UpdateSearchPath(const std::wstring& moduleFilename) const
	{
		auto parentPath = boost::filesystem::path(moduleFilename).parent_path();

		auto searchPath = defaultSearchPath_ + ';' + parentPath.string();
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
