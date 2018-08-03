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

#include <codecvt>
#include <unordered_set>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>

#include "SonarQubeExporter.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"
#include "CppCoverage/CoverageRateComputer.hpp"
#include "InvalidOutputFileException.hpp"

#include "Tools/Tool.hpp"

namespace cov = CppCoverage;
namespace property_tree = boost::property_tree;
namespace fs = boost::filesystem;

namespace Exporter
{
	namespace
	{
		//-------------------------------------------------------------------------
		std::wstring GetSafeFilePath(fs::path path)
		{
#ifdef _WIN32
			// SonarQube is case sensitive.
			// Find the "case correct" version of the path.
			WCHAR winPath[MAX_PATH];
			DWORD dwMax = MAX_PATH;
			HANDLE winHandle = FindFirstFileNameW(path.c_str(), 0, &dwMax, winPath);
			if (winHandle != INVALID_HANDLE_VALUE)
			{
				path = path.root_name() / fs::path(winPath);
				FindClose(winHandle);
			}
#endif
			// Set locale for conversion from UTF-8 to UTF-16.
			path.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>()));
			return path.wstring();
		}

		//-------------------------------------------------------------------------
		void FillCoverageTree(
			property_tree::wptree& root,
			const CppCoverage::CoverageData& coverageData)
		{
			auto& coverageTree = root.add_child(L"coverage", property_tree::wptree{});
			coverageTree.put(L"<xmlattr>.version", 1);

			for (const auto& module : coverageData.GetModules())
			{
				// Ignore empty modules
				if (module->GetFiles().empty())
					continue;

				const auto& modulePath = module->GetPath();
				for (const auto& file : module->GetFiles())
				{
					// Ignore empty files
					if (file->GetLines().empty())
						continue;

					auto& fileTree = coverageTree.add_child(L"file", property_tree::wptree{});
					auto path = GetSafeFilePath(file->GetPath());
					fileTree.put(L"<xmlattr>.path", path);

					for (const auto& line : file->GetLines())
					{
						auto& lineTree = fileTree.add_child(L"lineToCover", property_tree::wptree{});
						lineTree.put(L"<xmlattr>.lineNumber", line.GetLineNumber());
						lineTree.put(L"<xmlattr>.covered", line.HasBeenExecuted() ? true : false);
					}
				}
			}
		}
	}

	//-------------------------------------------------------------------------
	SonarQubeExporter::SonarQubeExporter() = default;

	//-------------------------------------------------------------------------
	boost::filesystem::path SonarQubeExporter::GetDefaultPath(const std::wstring& prefix) const
	{
		boost::filesystem::path path{ prefix };
		path += "Coverage.xml";
		return path;
	}

	//-------------------------------------------------------------------------
	void SonarQubeExporter::Export(
		const CppCoverage::CoverageData& coverageData,
		const boost::filesystem::path& output)
	{
		Tools::CreateParentFolderIfNeeded(output);
		std::wofstream ofs{ output.string().c_str() };
		if (!ofs)
			throw InvalidOutputFileException(output, "sonarqube");
		Export(coverageData, ofs);
		Tools::ShowOutputMessage(L"SonarQube report generated: ", output);
	}

	//-------------------------------------------------------------------------
	void SonarQubeExporter::Export(
		const CppCoverage::CoverageData& coverageData,
		std::wostream& ostream) const
	{
		using Ptree = property_tree::wptree;
		Ptree root;
		FillCoverageTree(root, coverageData);
		property_tree::xml_writer_settings<Ptree::key_type> settings(' ', 2);
		property_tree::xml_parser::write_xml(ostream, root, settings);
	}
}