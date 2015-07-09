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

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	struct ExecutedAddressManager::Instruction
	{
		explicit Instruction(unsigned char instruction)
			: instruction_(instruction)
			, hasBeenExecuted_(false)			
		{
		}

		unsigned char instruction_;
		bool hasBeenExecuted_;
	};
	
	//-------------------------------------------------------------------------
	struct ExecutedAddressManager::Line
	{
		bool HasLineBeenExecuted() const
		{
			for (const auto& instruction : instructions_)
			{
				if (!instruction)
					THROW(L"Null instruction");
				if (instruction->hasBeenExecuted_)
					return true;
			}
			return false;
		}

		// We cannot use vector because we will take the adress of the item
		std::list<ExecutedAddressManager::Instruction*> instructions_;
	};

	//-------------------------------------------------------------------------
	struct ExecutedAddressManager::File
	{		
		// We cannot use set because Line can be updated by MarkAddressAsExecuted
		std::map<unsigned int, Line> lines;
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
	{
	}
	
	//-------------------------------------------------------------------------
	ExecutedAddressManager::~ExecutedAddressManager()
	{
	}

	//-------------------------------------------------------------------------
	void ExecutedAddressManager::SetCurrentModule(const std::wstring& moduleName)
	{
		modules_.push_back(std::unique_ptr<Module>(new Module(moduleName)));
	}
	
	//-------------------------------------------------------------------------
	bool ExecutedAddressManager::RegisterAddress(
		const Address& address,
		const std::wstring& filename,
		unsigned int lineNumber, 
		unsigned char instructionValue)
	{
		auto& module = GetCurrentModule();
		auto& file = module.files_[filename];
		auto& lines = file.lines;
		auto& line = lines[lineNumber];

		LOG_TRACE << "RegisterAddress: " << address << " for " << filename << ":" << lineNumber;

		// Different {filename, line} can have the same address.
		// Same {filename, line} can have several addresses.		
		bool keepBreakpoint = false;
		Instruction* instruction = nullptr;
		auto itAddress = addressLineMap_.find(address);

		if (itAddress == addressLineMap_.end())
		{
			itAddress = addressLineMap_.emplace(address, Instruction{ instructionValue }).first;
			keepBreakpoint = true;
		}
		
		instruction = &itAddress->second;
		line.instructions_.push_back(instruction);
		
		return keepBreakpoint;
	}

	//-------------------------------------------------------------------------
	ExecutedAddressManager::Module& ExecutedAddressManager::GetCurrentModule()
	{
		if (modules_.empty())
			THROW("Not current module set");
		return *modules_.back();
	}

	//-------------------------------------------------------------------------
	boost::optional<unsigned char> ExecutedAddressManager::MarkAddressAsExecuted(
		const Address& address)
	{
		auto it = addressLineMap_.find(address);

		if (it == addressLineMap_.end())
			return boost::none;

		Instruction& instruction = it->second;
				
		instruction.hasBeenExecuted_ = true;
			 
		return instruction.instruction_;
	}
	
	//-------------------------------------------------------------------------
	CoverageData ExecutedAddressManager::CreateCoverageData(
		const std::wstring& name,
		int exitCode) const
	{
		CoverageData coverageData{ name, exitCode };
		
		for (const auto& module : modules_)
		{
			LOG_INFO << L"Create coverage report for " << module->name_;
			auto& moduleCoverage = coverageData.AddModule(module->name_);

			for (const auto& file : module->files_)
			{
				const std::wstring& name = file.first;
				const File& fileData = file.second;

				auto& fileCoverage = moduleCoverage.AddFile(name);

				for (const auto& pair : fileData.lines)
				{
					auto lineNumber = pair.first;
					const auto& line = pair.second;
					
					fileCoverage.AddLine(lineNumber, line.HasLineBeenExecuted());
				}
			}			
		}

		return coverageData;
	}
}
