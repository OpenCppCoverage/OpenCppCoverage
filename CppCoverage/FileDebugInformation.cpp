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
#include <boost/optional/optional.hpp>

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
			LineData(int lineNumber, DWORD64 lineAddress)
				: lineNumber_{ lineNumber }
				, lineAddress_{ lineAddress }
			{
			}

			int lineNumber_;
			DWORD64 lineAddress_;
		};

		//-----------------------------------------------------------------------
		struct LineContext
		{
			explicit LineContext(HANDLE hProcess) 
				: hProcess_{hProcess}
			{
			}

			HANDLE hProcess_;
			std::vector<LineData> lineDataCollection_;
			boost::optional<std::wstring> error_;
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
			{
				LOG_ERROR << "Invalid user context.";
				return FALSE;
			}

			lineContext->error_ = Tools::Try([&]() {
				if (!ExcludeLineInfo(*lineInfo, *lineContext))
				{
					lineContext->lineDataCollection_.emplace_back(
						lineInfo->LineNumber,
						lineInfo->Address);
				}
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

		struct ModuleAddress
		{
			HANDLE hProcess_;
			void* processBaseOfImage_;
			DWORD64 baseAddress_;
		};

		//-------------------------------------------------------------------------
		void HandleNewLine(
			const ModuleAddress& moduleAddress,
			const LineData& lineData,
			const std::wstring& filename,
			const std::set<int>& executableLinesSet,
			ICoverageFilterManager& coverageFilterManager,
			IDebugInformationEventHandler& debugInformationEventHandler)
		{
			auto lineNumber = lineData.lineNumber_;

			if (coverageFilterManager.IsLineSelected(filename, lineNumber, executableLinesSet))
			{
				auto addressValue = lineData.lineAddress_ - moduleAddress.baseAddress_
					+ reinterpret_cast<DWORD64>(moduleAddress.processBaseOfImage_);

				Address address{ moduleAddress.hProcess_,  reinterpret_cast<void*>(addressValue)};

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
		LineContext context{ hProcess_ };

		RetreiveLineData(filename, baseAddress, context);
		if (context.error_)
			throw std::runtime_error(Tools::ToLocalString(*context.error_));
		std::set<int> executableLinesSet;
		for (const auto& lineData : context.lineDataCollection_)
			executableLinesSet.insert(lineData.lineNumber_);
		LOG_DEBUG << L"Executable lines for " << filename << L": ";
		LOG_DEBUG << executableLinesSet;

		ModuleAddress moduleAddress;

		moduleAddress.hProcess_ = hProcess_;
		moduleAddress.processBaseOfImage_ = processBaseOfImage;
		moduleAddress.baseAddress_ = baseAddress;

		for (const auto& lineData : context.lineDataCollection_)
		{
			HandleNewLine(
				moduleAddress,
				lineData, 
				filename, 
				executableLinesSet,
				coverageFilterManager, 
				debugInformationEventHandler);
		}
	}	
}
