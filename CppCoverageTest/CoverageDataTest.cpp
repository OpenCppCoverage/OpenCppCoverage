#include "stdafx.h"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		const std::wstring moduleName = L"moduleName";
		const std::wstring filename = L"filename";

		void FillCoverageData(cov::CoverageData& data)
		{
			auto& createdModule = data.AddModule(moduleName);
			auto& createdFile = createdModule.AddFile(filename);
			createdFile.AddLine(1, true);
			createdFile.AddLine(2, false);
		}

		void CheckCoverageData(const cov::CoverageData& data)
		{
			const auto& modules = data.GetModules();
			ASSERT_EQ(1, modules.size());

			const auto& module = *modules.front();
			ASSERT_EQ(moduleName, module.GetName());

			const auto& files = module.GetFiles();
			ASSERT_EQ(1, files.size());

			const auto& file = *files.front();
			ASSERT_EQ(filename, file.GetFilename());
			ASSERT_EQ(nullptr, file[0]);

			const auto* line1 = file[1];
			const auto* line2 = file[2];

			ASSERT_NE(nullptr, line1);
			ASSERT_TRUE(line1->HasBeenExecuted());
			ASSERT_NE(nullptr, line2);
			ASSERT_FALSE(line2->HasBeenExecuted());
		}
	}

	TEST(CoverageDataTest, CoverageData)
	{
		cov::CoverageData data;
		
		FillCoverageData(data);
		CheckCoverageData(data);
	}

	TEST(CoverageDataTest, MoveConstructor)
	{
		cov::CoverageData data;

		FillCoverageData(data);

		cov::CoverageData movedCoverageData = { std::move(data) };
		CheckCoverageData(movedCoverageData);
	}
}