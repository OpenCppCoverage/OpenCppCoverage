#include "stdafx.h"
#include "Tools.hpp"

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
			explicit DebugEventsHandler(Tools::T_HandlesFct action)
			: action_(action)
			{
			}

			virtual void OnCreateProcess(const CREATE_PROCESS_DEBUG_INFO& processInfo)
			{
				action_(processInfo.hProcess, processInfo.hFile);
			}

		private:
			Tools::T_HandlesFct action_;
		};

	}
	//-------------------------------------------------------------------------
	std::wstring Tools::GetConsoleForCppCoverageTest()
	{		
		bfs::path path = { L"../Debug/ConsoleForCppCoverageTest.exe" };
		return bfs::canonical(path).wstring();
	}	

	//-------------------------------------------------------------------------
	void Tools::GetHandles(const std::wstring& filename, T_HandlesFct action)
	{		
		cov::StartInfo startInfo{ filename };
		cov::Debugger debugger;
		DebugEventsHandler debugEventsHandler{ action };

		debugger.Debug(startInfo, debugEventsHandler);
	}				
}

