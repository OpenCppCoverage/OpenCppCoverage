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

#pragma once

#include <iosfwd> 

#include "../ExporterExport.hpp"

namespace CppCoverage
{
	class FileCoverage;
}

namespace Exporter
{
	class EXPORTER_DLL HtmlFileCoverageExporter
	{
	public:
		static const std::wstring StyleBackgroundColorExecuted;
		static const std::wstring StyleBackgroundColorUnexecuted;
		static const std::wstring EndStyle;

	public:
		HtmlFileCoverageExporter(
			int maxSourceLineCount = 8000, 
			int maxSourceLineStyleChangesCount = 1000,
			int maxStyleChangesCount = 2000);

		bool Export(
			const CppCoverage::FileCoverage&,
			std::wostream& output) const;
		
		bool MustEnableCodePrettify(int lineCount, int styleChangedCount) const;

	private:
		HtmlFileCoverageExporter(const HtmlFileCoverageExporter&) = delete;
		HtmlFileCoverageExporter& operator=(const HtmlFileCoverageExporter&) = delete;

		int maxSourceLineCount_;
		int maxSourceLineStyleChangesCount_;
		int maxStyleChangesCount_;
	};
}


