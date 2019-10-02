// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "FileFilterExport.hpp"
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <filesystem>

namespace FileFilter
{	
	class IRelocationsExtractor;
	class ModuleInfo;
	class FileInfo;
	class LineInfo;

	class FILEFILTER_DLL ReleaseCoverageFilter
	{
	public:
		ReleaseCoverageFilter();
		explicit ReleaseCoverageFilter(std::unique_ptr<IRelocationsExtractor>);
		~ReleaseCoverageFilter();

		bool IsLineSelected(const ModuleInfo&, const FileInfo&, const LineInfo&);
		
	private:
		ReleaseCoverageFilter(const ReleaseCoverageFilter&) = delete;
		ReleaseCoverageFilter& operator=(const ReleaseCoverageFilter&) = delete;
		ReleaseCoverageFilter(ReleaseCoverageFilter&&) = delete;
		ReleaseCoverageFilter& operator=(ReleaseCoverageFilter&&) = delete;

		void UpdateCachesIfExpired(const ModuleInfo&, const FileInfo&);
		struct FileData;
		std::unique_ptr<FileData>
		UpdateLineDataCaches(const std::filesystem::path& filePath,
		                     const std::vector<LineInfo>&);

		const std::unique_ptr<IRelocationsExtractor> relocationsExtractor_;

		struct FileData
		{
			std::filesystem::path path_;
			std::unordered_set<DWORD64> lastSymbolAddresses_;
			std::unordered_map<int, int> addressCountByLine_;
		};

		struct ModuleData
		{
			std::filesystem::path path_;
			std::unordered_set<DWORD64> relocations_;
			std::unique_ptr<FileData> fileData_;
		};

		std::unique_ptr<ModuleData> mModuleData_;
	};
}

