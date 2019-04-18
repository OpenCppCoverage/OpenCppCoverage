// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2017 OpenCppCoverage
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
#include "Tools/MappedFile.hpp"
#include <fstream>

#include "TestHelper/TemporaryPath.hpp"

namespace ToolsTests
{
	namespace
	{
		//---------------------------------------------------------------------
		std::unique_ptr<TestHelper::TemporaryPath> CreateFile(const std::vector<std::string>& lines)
		{
			auto path = std::make_unique<TestHelper::TemporaryPath>();
			std::ofstream ofs(path->GetPath().string(), std::ios::binary);

			for (const auto& line : lines)
				ofs.write(line.c_str(), line.size());
			return path;
		}

		//---------------------------------------------------------------------
		std::vector<std::string> GetLines(const std::filesystem::path& path)
		{
			std::ifstream ifs(path.wstring());
			std::vector<std::string> lines;
			std::string line;

			while (std::getline(ifs, line))
				lines.push_back(line);
			return lines;
		}
	}

	//---------------------------------------------------------------------
	TEST(MappedFileTest, GetLine)
	{
		auto path = CreateFile({ "\r\n", "abc\r\n", "123\n" });
		auto file = Tools::MappedFile::TryCreate(path->GetPath());
		auto expectedLines = GetLines(*path);
		ASSERT_EQ(expectedLines, file->GetLines());
	}

	//---------------------------------------------------------------------
	TEST(MappedFileTest, EmptyFile)
	{
		TestHelper::TemporaryPath path{ TestHelper::TemporaryPathOption::CreateAsFile };
		ASSERT_EQ(nullptr, Tools::MappedFile::TryCreate(path.GetPath()));
	}

	//---------------------------------------------------------------------
	TEST(MappedFileTest, MissingFile)
	{
		ASSERT_EQ(nullptr, Tools::MappedFile::TryCreate("MissingFile"));
	}

	//---------------------------------------------------------------------
	TEST(MappedFileTest, WholeFile)
	{
		auto lines = GetLines(__FILE__);
		auto file = Tools::MappedFile::TryCreate(__FILE__);

		ASSERT_LT(0, static_cast<int>(lines.size()));
		ASSERT_EQ(lines, file->GetLines());
	}

	//---------------------------------------------------------------------
	TEST(MappedFileTest, EmptyEndLine)
	{
		auto path = CreateFile({ "Test", "\n", "\n" });
		auto expectedLines = GetLines(*path);
		auto file = Tools::MappedFile::TryCreate(*path);

		ASSERT_TRUE(file);
		ASSERT_EQ(expectedLines, file->GetLines());
	}
}