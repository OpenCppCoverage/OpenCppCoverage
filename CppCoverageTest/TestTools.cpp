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
#include "TestTools.hpp"

#include <filesystem>
#include <boost/algorithm/string.hpp>

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Debugger.hpp"
#include "CppCoverage/IDebugEventsHandler.hpp"
#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/CoverageFilterSettings.hpp"
#include "CppCoverage/RunCoverageSettings.hpp"

#include "TestCoverageConsole/TestCoverageConsole.hpp"

#include "Tools/WarningManager.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		struct DebugEventsHandler : public cov::IDebugEventsHandler
		{
			explicit DebugEventsHandler(TestTools::T_HandlesFct action)
			: action_(action)
			{
			}

			virtual void OnCreateProcess(HANDLE hProcess, const wchar_t* pszImageName, void* lpBaseOfImage) override
			{
				action_(hProcess,pszImageName);
			}

		private:
			TestTools::T_HandlesFct action_;
		};

	}

	namespace TestTools
	{
		//-------------------------------------------------------------------------
		void GetHandles(const std::filesystem::path& path, TestTools::T_HandlesFct action)
		{
			cov::StartInfo startInfo{ path };
			cov::Debugger debugger{ false, false, false };
			DebugEventsHandler debugEventsHandler{ action };

			debugger.Debug(startInfo, debugEventsHandler);
		}

		//---------------------------------------------------------------------
		boost::optional<cov::Options> Parse(
			const cov::OptionsParser& parser,
			const std::vector<std::string>& arguments,
			bool appendProgramToRun,
			std::wostream* emptyOptionsExplanation)
		{
			std::vector<const char*> argv;

			argv.push_back("programName");
			for (const auto& argument : arguments)
				argv.push_back(argument.c_str());

			auto programToRun = GetProgramToRun();
			if (appendProgramToRun)
				argv.push_back(programToRun.c_str());
			return parser.Parse(static_cast<int>(argv.size()), &argv[0], emptyOptionsExplanation);
		}

		const std::string GetOptionPrefix() { return "--"; }
		const std::string GetProgramToRun() { return TestCoverageConsole::GetOutputBinaryPath().string(); }

		//---------------------------------------------------------------------
		CoverageArgs::CoverageArgs(
			const std::vector<std::wstring>& arguments,
			const std::wstring& modulePattern,
			const std::wstring& sourcePattern)
			: programToRun_{ TestCoverageConsole::GetOutputBinaryPath() },
			arguments_{ arguments },
			modulePatternCollection_{ modulePattern },
			sourcePatternCollection_{ sourcePattern }
		{
		}

		//---------------------------------------------------------------------
		Plugin::CoverageData ComputeCoverageData(
			const std::vector<std::wstring>& arguments,
			const std::wstring& modulePattern,
			const std::wstring& sourcePattern)
		{
			CoverageArgs args{ arguments, modulePattern, sourcePattern };

			return ComputeCoverageDataPatterns(args);
		}

		//---------------------------------------------------------------------
		Plugin::CoverageData ComputeCoverageDataPatterns(
			const CoverageArgs& args)
		{
			cov::CodeCoverageRunner codeCoverageRunner{
			    std::make_shared<Tools::WarningManager>()};
			cov::Patterns modulePatterns{false};
			cov::Patterns sourcePatterns{ false };

			for (auto modulePattern : args.modulePatternCollection_)
			{
				boost::to_lower(modulePattern);
				modulePatterns.AddSelectedPatterns(modulePattern);
			}

			for (auto sourcePattern : args.sourcePatternCollection_)
			{
				boost::to_lower(sourcePattern);
				sourcePatterns.AddSelectedPatterns(sourcePattern);
			}

			cov::CoverageFilterSettings coverageFilterSettings{ modulePatterns, sourcePatterns };
			cov::StartInfo startInfo{ args.programToRun_ };

			for (const auto& argument : args.arguments_)
				startInfo.AddArgument(argument);

			cov::RunCoverageSettings settings(
				startInfo,
				coverageFilterSettings,
				args.unifiedDiffSettingsCollection_,
				args.excludedLineRegexes_,
				args.substitutePdbSourcePath_);
			settings.SetCoverChildren(args.coverChildren_);
			settings.SetContinueAfterCppException(args.continueAfterCppException_);
			settings.SetOptimizedBuildSupport(args.optimizedBuildSupport_);

			auto coverageData = codeCoverageRunner.RunCoverage(settings);

			return coverageData;
		}
	}
}

