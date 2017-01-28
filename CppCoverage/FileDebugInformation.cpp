// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage
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
#include "FileDebugInformation.hpp"

#include <set>
#include <boost/optional/optional.hpp>

#include "Tools/DbgHelp.hpp"
#include "Tools/Tool.hpp"
#include "Tools/Log.hpp"

#include "CppCoverageException.hpp"
#include "IDebugInformationEventHandler.hpp"
#include "ICoverageFilterManager.hpp"
#include "Address.hpp"

#include "FileFilter/ModuleInfo.hpp"
#include "FileFilter/FileInfo.hpp"
#include "FileFilter/LineInfo.hpp"

namespace CppCoverage
{
	namespace
	{
		//-----------------------------------------------------------------------
		struct LineContext
		{
			explicit LineContext(HANDLE hProcess) 
				: hProcess_{hProcess}
			{
			}

			HANDLE hProcess_;
			std::vector<FileFilter::LineInfo> lineInfoCollection_;
			boost::optional<std::wstring> error_;
		};

		//-----------------------------------------------------------------------
		bool TryAddLineInfo(const SRCCODEINFO& lineInfo, LineContext& context)
		{
			const int NoSource = 0x00feefee;
			if (lineInfo.LineNumber == NoSource)
				return false;

			char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
			PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);

			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = MAX_SYM_NAME;
			if (!SymFromAddr(context.hProcess_, lineInfo.Address, 0, symbol))
				THROW("Error when calling SymFromAddr");

			context.lineInfoCollection_.emplace_back(
				lineInfo.LineNumber,
				lineInfo.Address,
				symbol->Index,
				Tools::LocalToWString(symbol->Name));

			return true;
		}

		//---------------------------------------------------------------------
		BOOL CALLBACK SymEnumLinesProc(PSRCCODEINFO lineInfo, PVOID context)
		{
			auto lineContext = static_cast<LineContext*>(context);

			if (!lineContext)
			{
				LOG_ERROR << "Invalid user context.";
				return FALSE;
			}

			lineContext->error_ = Tools::Try([&]() {
				TryAddLineInfo(*lineInfo, *lineContext);
			});

			return lineContext->error_ ? FALSE: TRUE;
		}		
		
		//-------------------------------------------------------------------------
		void RetreiveLineData(
			const std::wstring& filename, 
			DWORD64 baseAddress,
			LineContext& context)
		{
			auto filenameStr = Tools::ToLocalString(filename);

			if (!SymEnumSourceLines(
				context.hProcess_,
				baseAddress,
				nullptr,
				filenameStr.c_str(),
				0,
				ESLFLAG_FULLPATH,
				SymEnumLinesProc,
				&context))
			{
				THROW("Cannot enumerate source lines for" << filename);
			}
		}

		//-------------------------------------------------------------------------
		void HandleNewLine(
			const FileFilter::ModuleInfo& moduleInfo,
			const FileFilter::FileInfo& fileInfo,
			const FileFilter::LineInfo& lineInfo,
			ICoverageFilterManager& coverageFilterManager,
			IDebugInformationEventHandler& debugInformationEventHandler)
		{
			auto lineNumber = lineInfo.lineNumber_;

			if (coverageFilterManager.IsLineSelected(moduleInfo, fileInfo, lineInfo))
			{
				auto addressValue = lineInfo.lineAddress_ - moduleInfo.baseAddress_
					+ reinterpret_cast<DWORD64>(moduleInfo.baseOfImage_);

				Address address{ moduleInfo.hProcess_,  reinterpret_cast<void*>(addressValue)};

				debugInformationEventHandler.OnNewLine(fileInfo.filePath_.wstring(), lineNumber, address);
			}
		}
	}

	//-------------------------------------------------------------------------
	void FileDebugInformation::LoadFile(
		const FileFilter::ModuleInfo& moduleInfo,
		const std::wstring& filePath,
		ICoverageFilterManager& coverageFilterManager,
		IDebugInformationEventHandler& debugInformationEventHandler) const
	{		
		LineContext context{ moduleInfo.hProcess_ };

		RetreiveLineData(filePath, moduleInfo.baseAddress_, context);
		if (context.error_)
			throw std::runtime_error(Tools::ToLocalString(*context.error_));

		FileFilter::FileInfo fileInfo{ filePath, std::move(context.lineInfoCollection_) };

		for (const auto& lineInfo : fileInfo.lineInfoColllection_)
		{
			HandleNewLine(
				moduleInfo,
				fileInfo, 
				lineInfo, 
				coverageFilterManager, 
				debugInformationEventHandler);
		}
	}	
}
