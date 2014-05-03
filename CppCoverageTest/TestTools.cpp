#include "stdafx.h"
#include "TestTools.hpp"

#include <boost/filesystem.hpp>

#include "CppCoverage/StartInfo.hpp"
#include "CppCoverage/Debugger.hpp"
#include "CppCoverage/IDebugEventsHandler.hpp"

namespace bfs = boost::filesystem;
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

			virtual void OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO& processInfo) override
			{
				action_(processInfo.hProcess, processInfo.hFile);
			}

		private:
			TestTools::T_HandlesFct action_;
		};

	}
		
	//-------------------------------------------------------------------------
	void TestTools::GetHandles(const boost::filesystem::path& path, T_HandlesFct action)
	{		
		cov::StartInfo startInfo{ path };
		cov::Debugger debugger;
		DebugEventsHandler debugEventsHandler{ action };

		debugger.Debug(startInfo, debugEventsHandler);
	}				
}

