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
#include <boost/algorithm/string.hpp>

#include "Tools/DbgHelp.hpp"
#include "Tools/Tool.hpp"
#include "Tools/Log.hpp"

#include "CppCoverageException.hpp"
#include "IDebugInformationEventHandler.hpp"
#include "ICoverageFilterManager.hpp"
#include "Address.hpp"

namespace CppCoverage
{
	namespace
	{
		//-----------------------------------------------------------------------
		struct LineData
		{
			LineData(int lineNumber, void* address)
				: lineNumber_{ lineNumber }
				, address_{ address }
			{
			}

			int lineNumber_;
			void* address_;
		};

		//-----------------------------------------------------------------------
		struct LineContext
		{
			LineContext(HANDLE hProcess, void* processBaseOfImage) 
				: hProcess_{hProcess}
				, processBaseOfImage_{ processBaseOfImage }
			{
			}

			HANDLE hProcess_;
			void* processBaseOfImage_;
			std::vector<LineData> lineDataCollection_;
		};

		//-----------------------------------------------------------------------
		bool ExcludeLineInfo(const SRCCODEINFO& lineInfo, const LineContext& context)
		{
			const int NoSource = 0x00feefee;
			if (lineInfo.LineNumber == NoSource)
				return true;

			char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
			PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);

			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = MAX_SYM_NAME;
			if (!SymFromAddr(context.hProcess_, lineInfo.Address, 0, symbol))
				THROW("Error when calling SymFromAddr");

			// Exclude compiler internal symbols.
			return boost::algorithm::starts_with(symbol->Name, "__");
		}

		//---------------------------------------------------------------------
		BOOL CALLBACK SymEnumLinesProc(PSRCCODEINFO lineInfo, PVOID context)
		{
			auto lineContext = static_cast<LineContext*>(context);

			if (!lineContext)
				THROW("Invalid user context.");

			if (ExcludeLineInfo(*lineInfo, *lineContext))
				return TRUE;

			auto lineNumber = lineInfo->LineNumber;

			DWORD64 addressValue = lineInfo->Address - lineInfo->ModBase 
				+ reinterpret_cast<DWORD64>(lineContext->processBaseOfImage_);
			lineContext->lineDataCollection_.emplace_back(lineNumber, reinterpret_cast<void*>(addressValue));
			return TRUE;
		}		
		
		//-------------------------------------------------------------------------
		void RetreiveLineData(
			const std::wstring& filename, 
			DWORD64 baseAddress,
			LineContext& context)
		{
			if (!SymEnumSourceLines(
				context.hProcess_,
				baseAddress,
				nullptr,
				Tools::ToString(filename).c_str(),
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
			HANDLE hProcess,
			const LineData& lineData,
			const std::wstring& filename,
			const std::set<int>& executableLinesSet,
			ICoverageFilterManager& coverageFilterManager,
			IDebugInformationEventHandler& debugInformationEventHandler)
		{
			auto lineNumber = lineData.lineNumber_;

			if (coverageFilterManager.IsLineSelected(filename, lineNumber, executableLinesSet))
			{
				Address address{ hProcess, lineData.address_ };

				debugInformationEventHandler.OnNewLine(filename, lineNumber, address);
			}
		}
	}

	//-------------------------------------------------------------------------
	FileDebugInformation::FileDebugInformation(HANDLE hProcess)
		: hProcess_{ hProcess }
	{
	}

	//-------------------------------------------------------------------------
	void FileDebugInformation::LoadFile(
		void* processBaseOfImage,
		DWORD64 baseAddress,
		const std::wstring& filename,
		ICoverageFilterManager& coverageFilterManager,
		IDebugInformationEventHandler& debugInformationEventHandler) const
	{		
		LineContext context{ hProcess_, processBaseOfImage };

		RetreiveLineData(filename, baseAddress, context);
		std::set<int> executableLinesSet;
		for (const auto& lineData : context.lineDataCollection_)
			executableLinesSet.insert(lineData.lineNumber_);
		LOG_DEBUG << L"Executable lines for " << filename << L": ";
		LOG_DEBUG << executableLinesSet;

		for (const auto& lineData : context.lineDataCollection_)
		{
			HandleNewLine(hProcess_, lineData, filename, executableLinesSet,
				coverageFilterManager, debugInformationEventHandler);
		}
	}	
}
