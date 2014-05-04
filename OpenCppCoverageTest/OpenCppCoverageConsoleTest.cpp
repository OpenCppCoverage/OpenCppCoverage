#include "stdafx.h"

#include <boost/filesystem.hpp>
#include <Poco/Process.h>
#include <Poco/Pipe.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>

#include "TestCoverageConsole/TestCoverageConsole.hpp"
#include "OpenCppCoverage/OpenCppCoverage.hpp"

#include "tools/ScopedAction.hpp"
#include "Tools/Tool.hpp"

namespace fs = boost::filesystem;

namespace OpenCppCoverageTest
{	
	//-------------------------------------------------------------------------
	TEST(ConsoleTest, UnhandledException)
	{
		fs::path console = OpenCppCoverage::GetOutputBinaryPath();
		fs::path testExe = TestCoverageConsole::GetOutputBinaryPath();

		std::vector<std::string> arguments{ testExe.string(), Tools::ToString(TestCoverageConsole::TestThrowUnHandledException) };
		Poco::Pipe output;
		auto handle = Poco::Process::launch(console.string(), arguments, "../Debug", nullptr, &output, &output);
		handle.wait(); // $$ check return value

		Poco::PipeInputStream inputStream(output);
		Poco::StreamCopier::copyStream(inputStream, std::cout);
	}	
}