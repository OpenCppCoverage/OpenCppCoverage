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

namespace boost
{
	namespace filesystem
	{
		class path;
	}
}

namespace FileFilter
{
	class File;

	class FILEFILTER_DLL UnifiedDiffParser
	{
	public:
		const static std::wstring FromFilePrefix;
		const static std::wstring ToFilePrefix;

		UnifiedDiffParser() = default;
				
		std::vector<File> Parse(std::wistream&) const;		

	private:
		UnifiedDiffParser(const UnifiedDiffParser&) = delete;
		UnifiedDiffParser& operator=(const UnifiedDiffParser&) = delete;
		UnifiedDiffParser(UnifiedDiffParser&&) = delete;
		UnifiedDiffParser& operator=(UnifiedDiffParser&&) = delete;

		struct HunksDifferences;
		struct Stream;

		boost::filesystem::path ExtractTargetFile(Stream&) const;
		HunksDifferences ExtractHunksDifferences(
							const Stream&, 
							const std::wstring& hunksDifferencesLine) const;

		std::vector<int> ExtractUpdatedLines(
							Stream&, 
							const std::wstring& hunksDifferencesLine) const;

		void ThrowError(const Stream&, const std::wstring&) const;

		void FillUpdatedLines(
			const std::wstring& line,
			std::vector<File>& files,
			Stream& stream) const;
	};
}

