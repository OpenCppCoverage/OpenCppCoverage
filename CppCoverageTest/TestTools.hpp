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

#include <Windows.h>

#include <string>
#include <functional>
#include <filesystem>

#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/Options.hpp"
#include "CppCoverage/SubstitutePdbSourcePath.hpp"

namespace Plugin
{
	class CoverageData;
}

namespace CppCoverageTest
{
	namespace TestTools
	{
		using T_HandlesFct = std::function<void(HANDLE hProcess, const wchar_t* pszFile)>;

		void GetHandles(const std::filesystem::path&, T_HandlesFct);

		boost::optional<CppCoverage::Options> Parse(
			const CppCoverage::OptionsParser& parser,
			const std::vector<std::string>& arguments,
			bool appendProgramToRun = true,
			std::wostream* emptyOptionsExplanation = nullptr);

		const std::string GetOptionPrefix();
		const std::string GetProgramToRun();

		//---------------------------------------------------------------------
		struct CoverageArgs
		{
			CoverageArgs(
				const std::vector<std::wstring>& arguments,
				const std::wstring& modulePattern,
				const std::wstring& sourcePattern);

			std::filesystem::path programToRun_;
			std::vector<std::wstring> arguments_;
			std::vector<std::wstring> modulePatternCollection_;
			std::vector<std::wstring> sourcePatternCollection_;
			std::vector<CppCoverage::UnifiedDiffSettings> unifiedDiffSettingsCollection_;
			bool coverChildren_ = true;
			bool continueAfterCppException_ = false;
			bool optimizedBuildSupport_ = false;
			std::vector<std::wstring> excludedLineRegexes_;
			std::vector<CppCoverage::SubstitutePdbSourcePath> substitutePdbSourcePath_;
		};

		//---------------------------------------------------------------------
		Plugin::CoverageData ComputeCoverageData(
			const std::vector<std::wstring>& arguments,
			const std::wstring& modulePattern,
			const std::wstring& sourcePattern);

		//---------------------------------------------------------------------
		Plugin::CoverageData ComputeCoverageDataPatterns(const CoverageArgs& args);
	}
}

