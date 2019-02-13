//// OpenCppCoverage is an open source code coverage for C++.
//// Copyright (C) 2017 OpenCppCoverage
////
//// This program is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// any later version.
////
//// This program is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "DebugInformationEnumerator.hpp"

#include <Windows.h>
#include <diacreate.h>
#include <dia2.h>
#include <atlbase.h>

#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>

#include "tools/Log.hpp"

#include "CppCoverageException.hpp"

namespace CppCoverage
{
	namespace
	{
		//----------------------------------------------------------------------
		class DiaString
		{
		  public:
			DiaString() = default;
			DiaString(const DiaString&) = delete;
			DiaString(DiaString&&) = delete;

			DiaString& operator=(const DiaString&) = delete;
			DiaString& operator=(DiaString&&) = delete;

			~DiaString()
			{
				ReleaseMemory();
			}

			BSTR* operator&()
			{
				ReleaseMemory();
				return &str_;
			}

			operator std::wstring() const
			{
				if (!str_)
					return L"";
				return str_;
			}

		  private:
			void ReleaseMemory()
			{
				if (str_ != nullptr)
				{
					// Cannot call SysFreeString as dia generated invalid BSTR
					// for x64.
					LocalFree(str_ - 2);
					str_ = nullptr;
				}
			}

			BSTR str_ = nullptr;
		};

		//----------------------------------------------------------------------
		template <typename Value, typename Collection, typename Fct>
		void EnumerateCollection(Collection& collection, Fct fct)
		{
			CComPtr<Value> value;
			ULONG celtFetched;
			while (collection.Next(1, &value, &celtFetched) == S_OK &&
			       celtFetched == 1 && value != nullptr)
			{
				fct(*value);
				value = nullptr;
			}
		}

		//----------------------------------------------------------------------
		CComPtr<IDiaEnumSourceFiles> GetEnumSourceFiles(IDiaSession& session)
		{
			CComPtr<IDiaEnumTables> tables;
			if (session.getEnumTables(&tables) != S_OK || !tables)
				THROW("DIA: Cannot get tables");

			CComPtr<IDiaEnumSourceFiles> sourceFiles;

			EnumerateCollection<IDiaTable>(*tables, [&](IDiaTable& table) {
				if (!sourceFiles)
				{
					CComPtr<IDiaEnumSourceFiles> currentSourceFiles;
					if (table.QueryInterface(_uuidof(IDiaEnumSourceFiles),
					                         (void**)&currentSourceFiles) ==
					    S_OK)
					{
						sourceFiles = currentSourceFiles;
					}
				}
			});

			return sourceFiles;
		}

		//----------------------------------------------------------------------
		struct DiaLoadCallback : public IDiaLoadCallback
		{
			//------------------------------------------------------------------
			HRESULT STDMETHODCALLTYPE QueryInterface(
			    REFIID riid,
			    _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
			{
				if (!ppvObject)
					return E_POINTER;

				if (riid == IID_IDiaLoadCallback)
				{
					*ppvObject = this;
					return S_OK;
				}

				return E_NOINTERFACE;
			}

			//------------------------------------------------------------------
			ULONG STDMETHODCALLTYPE AddRef() override
			{
				return 1; // On stack
			}

			//------------------------------------------------------------------
			ULONG STDMETHODCALLTYPE Release() override
			{
				return 1; // On stack
			}

			//------------------------------------------------------------------
			HRESULT STDMETHODCALLTYPE NotifyDebugDir(BOOL fExecutable,
			                                         DWORD cbData,
			                                         BYTE* pbData) override
			{
				return S_OK;
			}

			//------------------------------------------------------------------
			HRESULT STDMETHODCALLTYPE NotifyOpenDBG(LPCOLESTR dbgPath,
			                                        HRESULT resultCode) override
			{
				return S_OK;
			}

			//------------------------------------------------------------------
			HRESULT STDMETHODCALLTYPE NotifyOpenPDB(LPCOLESTR pdbPath,
			                                        HRESULT resultCode) override
			{
				LOG_DEBUG << "Try to load pdb from " << pdbPath << ": "
				          << (resultCode == S_OK ? "Success" : "Failed");
				return S_OK;
			}

			//------------------------------------------------------------------
			HRESULT STDMETHODCALLTYPE RestrictRegistryAccess() override
			{
				return S_OK;
			}

			//------------------------------------------------------------------
			HRESULT STDMETHODCALLTYPE RestrictSymbolServerAccess() override
			{
				return S_OK;
			}
		};

		//----------------------------------------------------------------------
		CComPtr<IDiaDataSource>
		LoadDataForExe(const boost::filesystem::path& path)
		{
			CComPtr<IDiaDataSource> sourcePtr;

			auto msDia = L"msdia140.dll";
			if (NoRegCoCreate(msDia,
			                  _uuidof(DiaSourceAlt),
			                  _uuidof(IDiaDataSource),
			                  (void**)&sourcePtr) != S_OK)
			{
				THROW(std::wstring(L"DIA: Cannot register dia. Make sure ") +
				      msDia + L" is in the current directory.");
			}

			DiaLoadCallback diaLoadCallback;
			if (!sourcePtr ||
			    sourcePtr->loadDataForExe(
			        path.wstring().c_str(), nullptr, &diaLoadCallback) != S_OK)
			{
				return nullptr;
			}
			return sourcePtr;
		}
	}

	//--------------------------------------------------------------------------
	DebugInformationEnumerator::DebugInformationEnumerator(
	    const std::vector<SubstitutePdbSourcePath>& substitutePdbSourcePaths)
		: substitutePdbSourcePaths_{ substitutePdbSourcePaths }
	{
	}

	//-------------------------------------------------------------------------
	bool
	DebugInformationEnumerator::Enumerate(const boost::filesystem::path& path,
	                                      IDebugInformationHandler& handler)
	{
		auto sourcePtr = LoadDataForExe(path);

		if (!sourcePtr)
			return false;

		CComPtr<IDiaSession> sessionPtr;
		if (sourcePtr->openSession(&sessionPtr) != S_OK || !sessionPtr)
			THROW("DIA: Cannot open session.");

		auto sourceFiles = GetEnumSourceFiles(*sessionPtr);
		if (!sourceFiles)
			THROW("DIA: cannot get SourceFiles");

		EnumerateCollection<IDiaSourceFile>(
		    *sourceFiles, [&](IDiaSourceFile& sourceFile) {
			    auto filename = GetSourceFileName(sourceFile);
			    if (handler.IsSourceFileSelected(filename))
			    {
				    lines_.clear();
				    EnumLines(*sessionPtr, sourceFile, handler);
				    handler.OnSourceFile(filename, lines_);
			    }
		    });
		return true;
	}

	//----------------------------------------------------------------------
	void
	DebugInformationEnumerator::EnumLines(IDiaSession& session,
	                                      IDiaSourceFile& sourceFile,
	                                      IDebugInformationHandler& handler)
	{
		CComPtr<IDiaEnumSymbols> symbols;
		if (sourceFile.get_compilands(&symbols) != S_OK || !symbols)
			THROW("DIA: Cannot get compilands");

		EnumerateCollection<IDiaSymbol>(*symbols, [&](IDiaSymbol& symbol) {
			CComPtr<IDiaEnumLineNumbers> lines;

			if (session.findLines(&symbol, &sourceFile, &lines) != S_OK ||
			    !lines)
			{
				THROW("DIA: Cannot find lines");
			}

			EnumerateCollection<IDiaLineNumber>(
			    *lines, [&](IDiaLineNumber& lineNumber) {
				    OnNewLine(session, lineNumber, handler);
			    });
		});
	}

	//----------------------------------------------------------------------
	void
	DebugInformationEnumerator::OnNewLine(IDiaSession& session,
	                                      IDiaLineNumber& lineNumber,
	                                      IDebugInformationHandler& handler)
	{
		DWORD linenum = 0;
		if (lineNumber.get_lineNumber(&linenum) != S_OK)
			THROW("DIA: Cannot get line number");

		const auto invalidLine = 0x00f00f00;
		if (linenum != invalidLine)
		{
			ULONGLONG virtualAddress = 0;
			if (lineNumber.get_virtualAddress(&virtualAddress) != S_OK)
				THROW("DIA: Cannot get virtual address");

			CComPtr<IDiaSymbol> symbol;
			if (session.findSymbolByVA(
			        virtualAddress, SymTagEnum::SymTagNull, &symbol) != S_OK ||
			    !symbol)
			{
				THROW("DIA: Cannot find symbol");
			}

			unsigned long symIndex = 0;
			if (symbol->get_symIndexId(&symIndex) != S_OK)
				THROW("DIA: Cannot get symIndex");

			lines_.emplace_back(linenum, virtualAddress, symIndex);
		}
	}

	//----------------------------------------------------------------------
	boost::filesystem::path DebugInformationEnumerator::GetSourceFileName(
	    IDiaSourceFile& sourceFile) const
	{
		DiaString fileName;
		if (sourceFile.get_fileName(&fileName) != S_OK)
			THROW("DIA: Cannot get filename");
		std::wstring filenameStr = fileName;

		for (const auto& paths : substitutePdbSourcePaths_)
		{
			auto pdbStartPath = paths.GetPdbStartPath().wstring();

			if (boost::istarts_with(filenameStr, pdbStartPath))
			{
				auto remainingPath = filenameStr.substr(pdbStartPath.size());
				filenameStr = (paths.GetLocalPath() / remainingPath).wstring();
			}
		}

		return filenameStr;
	}
}
