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

#include "pch.h"

#include "Plugin/Exporter/CoverageData.hpp"
#include "Plugin/Exporter/ModuleCoverage.hpp"
#include "Plugin/Exporter/FileCoverage.hpp"
#include "Plugin/Exporter/LineCoverage.hpp"

namespace PluginTest
{
	namespace
	{
		const std::wstring moduleName = L"moduleName";
		const std::wstring filename = L"filename";

		//---------------------------------------------------------------------
		void FillCoverageData(Plugin::CoverageData& data)
		{
			auto& createdModule = data.AddModule(moduleName);
			auto& createdFile = createdModule.AddFile(filename);
			createdFile.AddLine(1, true);
			createdFile.AddLine(2, false);
		}

		//---------------------------------------------------------------------
		void CheckCoverageData(const Plugin::CoverageData& data)
		{
			const auto& modules = data.GetModules();
			ASSERT_EQ(1, modules.size());

			const auto& module = *modules.front();
			ASSERT_EQ(moduleName, module.GetPath());

			const auto& files = module.GetFiles();
			ASSERT_EQ(1, files.size());

			const auto& file = *files.front();
			ASSERT_EQ(filename, file.GetPath());
			ASSERT_EQ(nullptr, file[0]);

			const auto* line1 = file[1];
			const auto* line2 = file[2];

			ASSERT_NE(nullptr, line1);
			ASSERT_TRUE(line1->HasBeenExecuted());
			ASSERT_NE(nullptr, line2);
			ASSERT_FALSE(line2->HasBeenExecuted());
		}
	}

	//---------------------------------------------------------------------
	TEST(CoverageDataTest, CoverageData)
	{
		Plugin::CoverageData data{L"", 0};
		
		FillCoverageData(data);
		CheckCoverageData(data);
	}

	//---------------------------------------------------------------------
	TEST(CoverageDataTest, MoveConstructor)
	{
		Plugin::CoverageData data{L"", 0};

		FillCoverageData(data);

		Plugin::CoverageData movedCoverageData = { std::move(data) };
		CheckCoverageData(movedCoverageData);
	}			
}