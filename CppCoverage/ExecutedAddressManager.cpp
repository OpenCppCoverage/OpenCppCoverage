#include "stdafx.h"
#include "ExecutedAddressManager.hpp"

#include <unordered_map>
#include <set>

#include "CppCoverageException.hpp"
#include "ModuleCoverage.hpp"
#include "FileCoverage.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	struct ExecutedAddressManager::Line
	{		
		Line(char instruction)
			: instruction_(instruction)
			, hasBeenExecuted_(false)
		{
		}

		unsigned char instruction_;
		bool hasBeenExecuted_;
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
	void ExecutedAddressManager::RegisterAddress(
		void* address, 
		const std::wstring& filename, 
		unsigned int lineNumber, 
		unsigned char instruction)
	{
		auto& module = GetCurrentModule();
		auto& file = module.files_[filename];
		auto& lines = file.lines;

		if (lines.find(lineNumber) != lines.end())
			THROW(L"Line: " << lineNumber << L" in " << filename << L" already exists.");

		const auto& pair = lines.emplace(lineNumber, Line(instruction)); //$$ replace std::insert by emplace					
		
		Line& insertedLine = pair.first->second;
		addressLineMap_.insert(std::make_pair(address, &insertedLine));
	}

	//-------------------------------------------------------------------------
	ExecutedAddressManager::Module& ExecutedAddressManager::GetCurrentModule()
	{
		if (modules_.empty())
			THROW("Not current module set");
		return *modules_.back();
	}

	//-------------------------------------------------------------------------
	unsigned char ExecutedAddressManager::MarkAddressAsExecuted(void* address)
	{
		auto it = addressLineMap_.find(address);

		if (it == addressLineMap_.end())
			THROW("Address should be register first");

		Line* line = it->second;

		if (!line)
			THROW("Line cannot be null");

		line->hasBeenExecuted_ = true;
			 
		return line->instruction_;
	}

	// $$ mettre un flag pour savoir si le module a bien ete charger mais qu il n a pas de fichier source	
	//-------------------------------------------------------------------------
	CoverageData ExecutedAddressManager::CreateCoverageData() const
	{
		CoverageData coverageData;

		for (const auto& module : modules_)
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
					const auto& line = pair.second;
					fileCoverage.AddLine(lineNumber, line.hasBeenExecuted_);
				}
			}
		}

		return coverageData;
	}
}
