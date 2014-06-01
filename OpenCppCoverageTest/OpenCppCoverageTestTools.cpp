#include "stdafx.h"
#include "OpenCppCoverageTestTools.hpp"

#include <boost/filesystem.hpp>
#include <Poco/Process.h>

#include "Tools/Tool.hpp"
#include "CppCoverage/OptionsParser.hpp"

#include "OpenCppCoverage/OpenCppCoverage.hpp"

namespace OpenCppCoverageTest
{
	//---------------------------------------------------------------------
	int RunCoverageOn(
		std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const boost::filesystem::path& outputFolder,
		const boost::filesystem::path& programToRun,
		const std::vector<std::wstring>& arguments)
	{		
		coverageArguments.push_back({ CppCoverage::OptionsParser::OutputDirectoryOption, outputFolder.string() });		
		
		return RunCoverageOn(coverageArguments, programToRun, arguments);
	}

	//---------------------------------------------------------------------
	int RunCoverageOn(
		const std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const boost::filesystem::path& programToRun,
		const std::vector<std::wstring>& arguments)
	{
		std::vector<std::string> allCoverageArguments;
			
		for (const auto& keyValue : coverageArguments)
		{
			allCoverageArguments.push_back("--" + keyValue.first);
			allCoverageArguments.push_back(keyValue.second);
		}
				
		allCoverageArguments.push_back(programToRun.string());
		for (const auto& argument : arguments)
			allCoverageArguments.push_back(Tools::ToString(argument));

		boost::filesystem::path openCppCoverage = OpenCppCoverage::GetOutputBinaryPath();
		auto handle = Poco::Process::launch(openCppCoverage.string(), allCoverageArguments, ".", nullptr, nullptr, nullptr);
		int exitCode = handle.wait();

		return exitCode;
	}
}