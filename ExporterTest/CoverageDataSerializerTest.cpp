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

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"
#include "Exporter/Binary/CoverageDataSerializer.hpp"
#include "Exporter/Binary/CoverageDataDeserializer.hpp"

#include "Helpers.hpp"

namespace cov = CppCoverage;
using namespace testing;

namespace ExporterTest
{
	namespace
	{			
		//---------------------------------------------------------------------
		void CompareLine(
			const cov::LineCoverage& line1,
			const cov::LineCoverage& line2)
		{
			ASSERT_EQ(line1.GetLineNumber(), line2.GetLineNumber());
			ASSERT_EQ(line1.HasBeenExecuted(), line2.HasBeenExecuted());
		}

		//---------------------------------------------------------------------
		void CompareFile(
			const cov::FileCoverage& file1,
			const cov::FileCoverage& file2)
		{
			ASSERT_EQ(file1.GetPath(), file2.GetPath());
			AssertContainerEqual(file1.GetLines(), file2.GetLines(), CompareLine);
		}

		//---------------------------------------------------------------------
		void CompareModule(
			const cov::ModuleCoverage& module1,
			const cov::ModuleCoverage& module2)
		{
			ASSERT_EQ(module1.GetPath(), module2.GetPath());
			AssertContainerUniquePtrEqual(module1.GetFiles(), module2.GetFiles(), CompareFile);
		}

		//---------------------------------------------------------------------
		void Compare(
			const CppCoverage::CoverageData& coverageData,
			const CppCoverage::CoverageData& coverageDataRestored)
		{
			ASSERT_EQ(coverageData.GetName(), coverageDataRestored.GetName());
			ASSERT_EQ(coverageData.GetExitCode(), coverageDataRestored.GetExitCode());

			AssertContainerUniquePtrEqual(
				coverageData.GetModules(), 
				coverageDataRestored.GetModules(), 
				CompareModule);
		}

		//---------------------------------------------------------------------
		struct CoverageDataSerializerTest : testing::Test
		{
			CoverageDataSerializerTest()
				: mCoverageData{ L"Test", 42 }
			{
				mCoverageData.AddModule(L"module1");
				auto& module = mCoverageData.AddModule(L"module2");
				auto& file = module.AddFile(L"file");

				file.AddLine(10, true);
				file.AddLine(11, false);
			}

			cov::CoverageData mCoverageData;
		};
	}
	
	//-------------------------------------------------------------------------
	TEST_F(CoverageDataSerializerTest, SerializeAndDeserialize)
	{	
		std::stringstream sstream;		

		Exporter::CoverageDataSerializer serializer;				
		serializer.Serialize(mCoverageData, sstream);
		sstream.flush();

		Exporter::CoverageDataDeserializer deserializer;		
		auto coverageDataRestored = deserializer.Deserialize(sstream, "");
		
		Compare(mCoverageData, coverageDataRestored);
	}

	//-------------------------------------------------------------------------
	TEST_F(CoverageDataSerializerTest, InvalidFile)
	{
		Exporter::CoverageDataDeserializer deserializer;
		std::stringstream istr;

		std::default_random_engine generator;		
		std::uniform_int_distribution<int> distribution(0, 1);

		for (int i = 0; i < 100; ++i)
			istr << distribution(generator);

		ASSERT_THROW(deserializer.Deserialize(istr, "todo"), std::runtime_error);
	}
}