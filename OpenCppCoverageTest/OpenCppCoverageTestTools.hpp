#pragma once

#include <vector>
#include <string>

namespace boost
{
	namespace filesystem
	{
		class path;
	}
}

namespace OpenCppCoverageTest
{
	int RunCoverageOn(
		std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const boost::filesystem::path& outputFolder,
		const boost::filesystem::path& programToRun,
		const std::vector<std::wstring>& arguments);

	int RunCoverageOn(
		const std::vector<std::pair<std::string, std::string>>& coverageArguments,
		const boost::filesystem::path& programToRun,
		const std::vector<std::wstring>& arguments);
}