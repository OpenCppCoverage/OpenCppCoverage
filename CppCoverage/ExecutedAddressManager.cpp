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
#include "ExecutedAddressManager.hpp"

#include <unordered_map>
#include <set>

#include "tools/Log.hpp"

#include "CppCoverageException.hpp"
#include "ModuleCoverage.hpp"
#include "FileCoverage.hpp"
#include "Address.hpp"
#include "CoverageDataMerger.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	ExecutedAddressManager::Line::Line(unsigned char instructionToRestore)
		: instructionToRestore_{ instructionToRestore }
	{
	}

	//-------------------------------------------------------------------------
	struct ExecutedAddressManager::File
	{		
		// Use map to have iterator always valid
		std::map<unsigned int, bool> lines;
	};

	//-------------------------------------------------------------------------
	struct ExecutedAddressManager::Module
	{
		explicit Module(const std::wstring& name)
		: name_(name)
		{
		}

		std::wstring name_;
		std::unordered_map<std::wstring, File> files_;
	};
	
	//-------------------------------------------------------------------------
	ExecutedAddressManager::ExecutedAddressManager()
		: coverageData_{L"", 0}
	{
	}
	
	//-------------------------------------------------------------------------
	ExecutedAddressManager::~ExecutedAddressManager()
	{
	}

	//-------------------------------------------------------------------------
	void ExecutedAddressManager::AddModule(HANDLE hProcess, const std::wstring& moduleName)
	{
		modulesByHandle_[hProcess].push_back(std::make_unique<Module>(moduleName));
	}
	
	//-------------------------------------------------------------------------
	bool ExecutedAddressManager::RegisterAddress(
		const Address& address,
		const std::wstring& filename,
		unsigned int lineNumber, 
		unsigned char instructionValue)
	{
		auto& module = GetLastAddedModule(address.GetProcessHandle());
		auto& file = module.files_[filename];
		auto& lines = file.lines;

		LOG_TRACE << "RegisterAddress: " << address << " for " << filename << ":" << lineNumber;

		// Different {filename, line} can have the same address.
		// Same {filename, line} can have several addresses.		
		bool keepBreakpoint = false;
		auto itAddress = addressLineMap_.find(address);

		if (itAddress == addressLineMap_.end())
		{
			itAddress = addressLineMap_.emplace(address, Line{ instructionValue }).first;
			keepBreakpoint = true;
		}
		
		auto& line = itAddress->second;
		line.hasBeenExecutedCollection_.push_back(&lines[lineNumber]);
		
		return keepBreakpoint;
	}

	//-------------------------------------------------------------------------
	ExecutedAddressManager::Module& ExecutedAddressManager::GetLastAddedModule(HANDLE hProcess)
	{
		auto it = modulesByHandle_.find(hProcess);

		if (it == modulesByHandle_.end())
			THROW("Cannot get last added module (hProcess).");
		const auto& modules = it->second;

		if (modules.empty())
			THROW("Cannot get last added module (no module).");

		return *modules.back();
	}

	//-------------------------------------------------------------------------
	boost::optional<unsigned char> ExecutedAddressManager::MarkAddressAsExecuted(
		const Address& address)
	{
		auto it = addressLineMap_.find(address);

		if (it == addressLineMap_.end())
			return boost::none;

		auto& line = it->second;

		for (bool* hasBeenExecuted : line.hasBeenExecutedCollection_)
		{
			if (!hasBeenExecuted)
				THROW("Invalid pointer");
			*hasBeenExecuted = true;
		}
		return line.instructionToRestore_;
	}
	
	//-------------------------------------------------------------------------
	CoverageData ExecutedAddressManager::CreateCoverageData(const T_Modules& modules) const
	{
		CoverageData coverageData{ L"", 0 };

		for (const auto& module : modules)
		{
			auto& moduleCoverage = coverageData.AddModule(module->name_);

			for (const auto& file : module->files_)
			{
				const std::wstring& name = file.first;
				const File& fileData = file.second;

				auto& fileCoverage = moduleCoverage.AddFile(name);

				for (const auto& pair : fileData.lines)
				{
					auto lineNumber = pair.first;
					bool hasLineBeenExecuted = pair.second;
					
					fileCoverage.AddLine(lineNumber, hasLineBeenExecuted);
				}
			}			
		}

		return coverageData;
	}

	//-------------------------------------------------------------------------
	void ExecutedAddressManager::AddModulesToCoverageData(HANDLE hProcess)
	{
		auto it = modulesByHandle_.find(hProcess);

		if (it != modulesByHandle_.end())
		{
			CoverageDataMerger coverageDataMerger;

			auto processCoverageData = CreateCoverageData(it->second);
			modulesByHandle_.erase(it);
			std::vector<CoverageData> coverageDatas;
			coverageDatas.push_back(std::move(coverageData_));
			coverageDatas.push_back(std::move(processCoverageData));
			coverageData_ = coverageDataMerger.Merge(coverageDatas);
		}
	}

	//-------------------------------------------------------------------------
	void ExecutedAddressManager::OnExitProcess(HANDLE hProcess)
	{
		AddModulesToCoverageData(hProcess);

		auto it = addressLineMap_.begin();

		while (it != addressLineMap_.end())
		{
			const Address& address = it->first;

			if (address.GetProcessHandle() == hProcess)
				it = addressLineMap_.erase(it);
			else
				++it;
		}
	}

	//-------------------------------------------------------------------------
	CoverageData ExecutedAddressManager::CreateCoverageData(
		const std::wstring& name,
		int exitCode)
	{
		coverageData_.SetName(name);
		coverageData_.SetExitCode(exitCode);

		return std::move(coverageData_);
	}
}
