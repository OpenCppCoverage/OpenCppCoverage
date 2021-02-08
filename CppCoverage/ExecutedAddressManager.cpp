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
#include <boost/container/small_vector.hpp>

#include "tools/Log.hpp"

#include "CppCoverageException.hpp"
#include "Plugin/Exporter/ModuleCoverage.hpp"
#include "Plugin/Exporter/FileCoverage.hpp"
#include "Address.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	struct ExecutedAddressManager::Line
	{
		explicit Line(unsigned char instructionToRestore, void* dllBaseOfImage)
			: instructionToRestore_{ instructionToRestore }
			, dllBaseOfImage_{ dllBaseOfImage }
		{
		}

		const unsigned char instructionToRestore_;
		void* const dllBaseOfImage_;
		boost::container::small_vector<bool*, 1> hasBeenExecutedCollection_;
	};

	//-------------------------------------------------------------------------
	struct ExecutedAddressManager::File
	{		
		// Use map to have iterator always valid
		std::map<unsigned int, bool> lines;
	};

	//-------------------------------------------------------------------------
	struct ExecutedAddressManager::Module
	{
		explicit Module(const std::wstring& name) : name_{ name }
		{
		}

		const std::wstring name_;
		std::unordered_map<std::wstring, File> files_;
	};
	
	//-------------------------------------------------------------------------
	ExecutedAddressManager::ExecutedAddressManager()
	{
		lastModule_.baseOfImage_ = nullptr;
		lastModule_.module_ = nullptr;
	}

	//-------------------------------------------------------------------------
	ExecutedAddressManager::~ExecutedAddressManager()
	{
	}

	//-------------------------------------------------------------------------
	void ExecutedAddressManager::AddModuleFiles(const std::wstring& moduleName, const std::vector<std::filesystem::path>& filepaths)
	{
		auto module = modules_.find(moduleName);

		if(module == modules_.end())
			THROW("Cannot find module.");

		auto& files = module->second.files_;
		for (const auto& filepath : filepaths)
		{
			auto file = files.find(filepath.wstring());
			
			if (file == files.end())
			{
				files.emplace(filepath.wstring(), File{});
			}
		}
	}

	//-------------------------------------------------------------------------
	void ExecutedAddressManager::AddModule(
		const std::wstring& moduleName,
		void* dllBaseOfImage)
	{
		auto it = modules_.find(moduleName);

		if (it == modules_.end())
			it = modules_.emplace(moduleName, Module{ moduleName }).first;
		lastModule_.module_ = &it->second;
		lastModule_.baseOfImage_ = dllBaseOfImage;
	}
	
	//-------------------------------------------------------------------------
	bool ExecutedAddressManager::RegisterAddress(
		const Address& address,
		const std::wstring& filename,
		unsigned int lineNumber, 
		unsigned char instructionValue)
	{
		auto& module = GetLastAddedModule();
		auto& file = module.files_[filename];

		LOG_TRACE << "RegisterAddress: " << address << " for " << filename << ":" << lineNumber;

		// Different {filename, line} can have the same address.
		// Same {filename, line} can have several addresses.		
		bool keepBreakpoint = false;
		auto itAddress = addressLineMap_.find(address);

		if (itAddress == addressLineMap_.end())
		{
			itAddress = addressLineMap_.emplace(address, 
				Line{ instructionValue, lastModule_.baseOfImage_ }).first;
			keepBreakpoint = true;
		}
		
		auto& line = itAddress->second;
		line.hasBeenExecutedCollection_.push_back(&file.lines[lineNumber]);
		
		return keepBreakpoint;
	}

	//-------------------------------------------------------------------------
	ExecutedAddressManager::Module& ExecutedAddressManager::GetLastAddedModule()
	{
		if (!lastModule_.module_)
			THROW("Cannot get last module.");

		return *lastModule_.module_;
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
	Plugin::CoverageData ExecutedAddressManager::CreateCoverageData(
		const std::wstring& name,
		int exitCode) const
	{
		Plugin::CoverageData coverageData{ name, exitCode };

		for (const auto& pair : modules_)
		{
			const auto& module = pair.second;
			auto& moduleCoverage = coverageData.AddModule(module.name_);

			for (const auto& file : module.files_)
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
	template <typename Condition>
	void ExecutedAddressManager::RemoveAddressLineIf(Condition condition)
	{
		auto it = addressLineMap_.begin();

		while (it != addressLineMap_.end())
		{
			if (condition(*it))
				it = addressLineMap_.erase(it);
			else
				++it;
		}
	}

	//-------------------------------------------------------------------------
	void ExecutedAddressManager::OnExitProcess(HANDLE hProcess)
	{
		RemoveAddressLineIf([=](const auto& pair)
		{
			return pair.first.GetProcessHandle() == hProcess;
		});
	}

	//-------------------------------------------------------------------------
	void ExecutedAddressManager::OnUnloadModule(HANDLE hProcess, void* dllBaseOfImage)
	{
		RemoveAddressLineIf([=](const auto& pair)
		{
			return pair.first.GetProcessHandle() == hProcess
				&& pair.second.dllBaseOfImage_ == dllBaseOfImage;
		});
	}
}
