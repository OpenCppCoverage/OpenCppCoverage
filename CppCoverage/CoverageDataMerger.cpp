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
#include "CoverageDataMerger.hpp"

#include "CoverageData.hpp"
#include "ModuleCoverage.hpp"
#include "FileCoverage.hpp"
#include "LineCoverage.hpp"

namespace fs = boost::filesystem;

namespace CppCoverage
{
	namespace
	{
		//---------------------------------------------------------------------
		CoverageData CreateCoverageData(const std::vector<CoverageData>& coverageDataCollection)
		{
			std::wstring name;
			int lastNotZeroExitCode = 0;

			for (const auto& coverageData : coverageDataCollection)
			{
				if (!name.empty())
					name += L"-";
				name += coverageData.GetName();
				auto exitCode = coverageData.GetExitCode();
				if (exitCode)
					lastNotZeroExitCode = exitCode;
			}

			return CoverageData{ name, lastNotZeroExitCode };
		}
		
		//---------------------------------------------------------------------
		template <typename Object, typename Key, typename Child>
		std::map<Key, std::vector<Child*>> GroupChildrenByKey(
			const std::vector<Object>& collection,
			const std::function<const std::vector<std::unique_ptr<Child>>& (const Object&)>& getChildren,
			const std::function<const Key& (const Child&)>& getKey)
		{			
			std::map<Key, std::vector<Child*>> childrenByKey;

			for (const auto& object : collection)
			{					
				for (const auto& child : getChildren(object))
				{
					const auto& key = getKey(*child);

					childrenByKey[key].push_back(child.get());
				}
			}

			return childrenByKey;
		}
		
		//---------------------------------------------------------------------
		void FillFiles(
			FileCoverage& file,
			const std::vector<FileCoverage*>& files)
		{
			for (const auto& f : files)
			{
				for (const auto& line : f->GetLines())
				{
					auto lineNumber = line.GetLineNumber();
					auto hasBeenExecuted = line.HasBeenExecuted();

					if (!file[lineNumber])
						file.AddLine(lineNumber, hasBeenExecuted);
					else if (hasBeenExecuted)
						file.UpdateLine(lineNumber, true);
				}
			}
		}

		//---------------------------------------------------------------------
		void FillModule(
			ModuleCoverage& module,
			const std::vector<ModuleCoverage*>& modules)
		{
			std::map<fs::path, std::vector<FileCoverage*>> filesByPath =
				GroupChildrenByKey<ModuleCoverage*, fs::path, FileCoverage>(
				modules,
				[](const ModuleCoverage* m) -> const ModuleCoverage::T_FileCoverageCollection&{ return m->GetFiles(); },
				[](const FileCoverage& file) -> const fs::path&{ return file.GetPath(); });

			for (const auto& pair : filesByPath)
			{
				auto& file = module.AddFile(pair.first);
				FillFiles(file, pair.second);
			}
		}
	}

	//-------------------------------------------------------------------------
	CoverageData CoverageDataMerger::Merge(
		const std::vector<CoverageData>& coverageDataCollection) const
	{
		auto coverageData = CreateCoverageData(coverageDataCollection);

		std::map<fs::path, std::vector<ModuleCoverage*>> modulesByPath =
			GroupChildrenByKey<CoverageData, fs::path, ModuleCoverage>(
				coverageDataCollection,
				[](const CoverageData& data) -> const CoverageData::T_ModuleCoverageCollection& { return data.GetModules(); },
				[](const ModuleCoverage& module) -> const fs::path& { return module.GetPath(); });
		
		for (const auto& pair : modulesByPath)
		{
			auto& module = coverageData.AddModule(pair.first);
			FillModule(module, pair.second);
		}
		
		return coverageData;
	}
}
