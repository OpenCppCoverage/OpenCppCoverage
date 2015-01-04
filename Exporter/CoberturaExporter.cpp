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

#include <unordered_set>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>

#include "CoberturaExporter.hpp"
#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"

#include "Tools/Tool.hpp"

namespace cov = CppCoverage;
namespace property_tree = boost::property_tree;
namespace fs = boost::filesystem;

namespace Exporter
{
	namespace
	{
		//-------------------------------------------------------------------------
		property_tree::wptree& AddChild(property_tree::wptree& tree, const std::wstring& name)
		{
			return tree.add_child(name, property_tree::wptree{});
		}

		//-------------------------------------------------------------------------
		void FillFileTree(
			property_tree::wptree& fileTree, 
			const cov::FileCoverage& file,
			std::unordered_set<std::wstring>& rootPaths)
		{
			const auto& path = file.GetPath();
			auto res = path.relative_path();

			rootPaths.insert(path.root_name().wstring());
			fileTree.put(L"<xmlattr>.name", path.filename().wstring());
			fileTree.put(L"<xmlattr>.filename", path.relative_path().wstring());

			property_tree::wptree& linesTree = AddChild(fileTree, L"lines");

			for (const auto& line : file.GetLines())
			{
				property_tree::wptree& lineTree = AddChild(linesTree, L"line");

				lineTree.put(L"<xmlattr>.number", std::to_wstring(line.GetLineNumber()));
				lineTree.put(L"<xmlattr>.hits", line.HasBeenExecuted() ? L"1" : L"0");
			}
		}

		//-------------------------------------------------------------------------
		void WriteSourceRoots(
			property_tree::wptree& coverageTree,
			const std::unordered_set<std::wstring>& rootPaths)
		{			
			auto& sourcesTree = AddChild(coverageTree, L"sources");

			for (const auto& rootPath : rootPaths)
				sourcesTree.add(L"source", rootPath);
		}

		//-------------------------------------------------------------------------
		void FillCoverageTree(
			property_tree::wptree& root,
			const CppCoverage::CoverageData& coverageData)
		{
			auto& coverageTree = AddChild(root, L"coverage");
			property_tree::wptree& packagesTree = AddChild(coverageTree, L"packages");
			std::unordered_set<std::wstring> rootPaths;

			for (const auto& module : coverageData.GetModules())
			{
				// Do not create package if no files exists -> Coverage will not be visible by module
				if (!module->GetFiles().empty())
				{
					property_tree::wptree& packageTree = AddChild(packagesTree, L"package");
					property_tree::wptree& classesTree = AddChild(packageTree, L"classes");

					packageTree.put(L"<xmlattr>.name", module->GetPath().wstring());

					for (const auto& file : module->GetFiles())
					{
						property_tree::wptree& fileTree = AddChild(classesTree, L"class");
						FillFileTree(fileTree, *file, rootPaths);
					}
				}
			}
			WriteSourceRoots(coverageTree, rootPaths);
		}
	}

	//-------------------------------------------------------------------------
	CoberturaExporter::CoberturaExporter() = default;

	//-------------------------------------------------------------------------
	boost::filesystem::path CoberturaExporter::GetDefaultPath(const std::wstring& prefix) const
	{
		boost::filesystem::path path{ prefix };
		
		path += "Coverage.xml";

		return path;		
	}

	//-------------------------------------------------------------------------
	void CoberturaExporter::Export(
		const CppCoverage::CoverageData& coverageData, 
		const boost::filesystem::path& output) const
	{		
		std::wofstream ofs{ output.string().c_str() };

		Export(coverageData, ofs);
		Tools::ShowOutputMessage(L"Cobertura report generated: ", output);
	}

	//-------------------------------------------------------------------------
	void CoberturaExporter::Export(
		const CppCoverage::CoverageData& coverageData,
		std::wostream& ostream) const
	{
		property_tree::wptree root;
		
		FillCoverageTree(root, coverageData);

		property_tree::xml_writer_settings<wchar_t> settings(' ', 2);
		property_tree::xml_parser::write_xml(ostream, root, settings);
	}
}
