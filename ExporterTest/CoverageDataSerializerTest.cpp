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

#include <sstream>
#include <random>
#include <fstream>

#include "Plugin/Exporter/CoverageData.hpp"
#include "Plugin/Exporter/ModuleCoverage.hpp"
#include "Plugin/Exporter/FileCoverage.hpp"
#include "Plugin/Exporter/LineCoverage.hpp"
#include "Exporter/Binary/CoverageDataSerializer.hpp"
#include "Exporter/Binary/CoverageDataDeserializer.hpp"

#include "TestHelper/TemporaryPath.hpp"
#include "TestHelper/CoverageDataComparer.hpp"

using namespace testing;

namespace ExporterTest
{
	namespace
	{					
		//---------------------------------------------------------------------
		void AddRandomFiles(
			Plugin::ModuleCoverage& module,
			std::default_random_engine& generator,
			std::uniform_int_distribution<int>& distribution)
		{
			for (int fileIndex = 0; fileIndex < 10; ++fileIndex)
			{
				if (distribution(generator))
				{
					auto& file = module.AddFile(std::to_wstring(fileIndex));

					for (int line = 0; line < 100; ++line)
						file.AddLine(line, distribution(generator) != 0);
				}
			}
		}

		//---------------------------------------------------------------------
		Plugin::CoverageData CreateRandomCoverageData()
		{
			Plugin::CoverageData coverageData{ L"Testé", 42 };
			std::default_random_engine generator;
			std::uniform_int_distribution<int> distribution(0, 1);
			
			for (int moduleIndex = 0; moduleIndex < 100; ++moduleIndex)
			{
				if (distribution(generator))
				{
					auto& module = coverageData.AddModule(std::to_wstring(moduleIndex));
					AddRandomFiles(module, generator, distribution);
				}
			}

			coverageData.AddModule("éèà").AddFile("éèà").AddLine(0, true);

			return coverageData;
		}
	}
	
	//-------------------------------------------------------------------------
	TEST(CoverageDataSerializerTest, SerializeAndDeserialize)
	{
		TestHelper::TemporaryPath path;
		Exporter::CoverageDataSerializer serializer;
		auto randomCoverageData = CreateRandomCoverageData();

		serializer.Serialize(randomCoverageData, path.GetPath().string());

		Exporter::CoverageDataDeserializer deserializer;		
		auto coverageDataRestored = deserializer.Deserialize(path.GetPath().string(), "");

		TestHelper::CoverageDataComparer().AssertEquals(randomCoverageData, coverageDataRestored);
	}

	//-------------------------------------------------------------------------
	TEST(CoverageDataSerializerTest, InvalidFile)
	{
		Exporter::CoverageDataDeserializer deserializer;
		TestHelper::TemporaryPath path;
		std::ofstream ofs{ path.GetPath().string(), std::ios::binary};

		std::default_random_engine generator;		
		std::uniform_int_distribution<int> distribution(0, 1);

		for (int i = 0; i < 100; ++i)
			ofs << distribution(generator);

		ASSERT_THROW(deserializer.Deserialize(path.GetPath(), "todo"), std::runtime_error);
	}
}