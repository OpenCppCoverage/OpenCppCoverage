#include "stdafx.h"
#include "CodeCoverageRunner.hpp"

#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/filesystem.hpp>

#include "Tools/Log.hpp"

#include "Exporter/Html/HtmlExporter.hpp"

#include "CppCoverage/CodeCoverageRunner.hpp"
#include "CppCoverage/CoverageSettings.hpp"
#include "CppCoverage/Patterns.hpp"
#include "CppCoverage/StartInfo.hpp"

#include "VSPackageException.hpp"

#include <msclr/auto_gcroot.h>
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;

namespace VSPackageUnManagedWrapper
{
	namespace
	{	
		//---------------------------------------------------------------------
		String^ ToManagedString(const std::wstring& str)
		{
			return gcnew String(str.c_str());
		}

		//---------------------------------------------------------------------
		String^ ToManagedString(const std::string& str)
		{
			return gcnew String(str.c_str());
		}

		//---------------------------------------------------------------------
		std::wstring ToWString(String^ str)
		{
			return msclr::interop::marshal_as<std::wstring>(str);
		}

		//---------------------------------------------------------------------
		class OutputSink : 
			public boost::log::sinks::basic_formatted_sink_backend<
										wchar_t, 
										boost::log::sinks::synchronized_feeding>
		{
		public:
			//-----------------------------------------------------------------
			explicit OutputSink(ILogger^ logger)
				: logger_(logger)
			{
			}

			//-----------------------------------------------------------------
			void consume(const boost::log::record_view& rec, const string_type& commandLine)
			{
				auto message = ToManagedString(commandLine);
				logger_->WriteLine(message);
			}

		private:
			msclr::auto_gcroot<ILogger^> logger_;			
		};
		
		//---------------------------------------------------------------------
		void RedirectToLogger(ILogger^ logger)
		{
			typedef boost::log::sinks::synchronous_sink<OutputSink> sink_t;
			auto sink = boost::make_shared<sink_t>(logger);

			Tools::SetLoggerMinSeverity(boost::log::trivial::info);
			Tools::SetLogSink(sink);
		}

		//---------------------------------------------------------------------
		CppCoverage::CoverageData RunCoverage(
			String^ command,
			String^ arguments,
			String^ workingDir,
			Collections::Generic::IEnumerable<String^>^ modulePaths,
			Collections::Generic::IEnumerable<String^>^ sourcePaths)
		{
			CppCoverage::CodeCoverageRunner codeCoverageRunner;
			CppCoverage::Patterns modulePatterns;
			CppCoverage::Patterns sourcePatterns;

			for each(String^ path in modulePaths)
				modulePatterns.AddSelectedPatterns(ToWString(path));
			for each(String^ path in sourcePaths)
				sourcePatterns.AddSelectedPatterns(ToWString(path));

			CppCoverage::CoverageSettings settings {modulePatterns, sourcePatterns};			
			CppCoverage::StartInfo startInfo(ToWString(command));			
			
			startInfo.AddArguments(ToWString(arguments));			
			if (!String::IsNullOrEmpty(workingDir))
				startInfo.SetWorkingDirectory(ToWString(workingDir));			
			
			return codeCoverageRunner.RunCoverage(startInfo, settings);
		}		

		//---------------------------------------------------------------------
		boost::filesystem::path GetCurrentLibraryPath()
		{
			auto location = System::Reflection::Assembly::GetExecutingAssembly()->Location;
			boost::filesystem::path libraryPath = ToWString(location);

			return libraryPath.parent_path();
		}
	}

	//---------------------------------------------------------------------
	String^ CodeCoverageRunner::Run(
		String^ command,
		String^ arguments,
		String^ workingDir,
		Collections::Generic::IEnumerable<String^>^ modulePaths,
		Collections::Generic::IEnumerable<String^>^ sourcePaths,
		ILogger^ logger)
	{		
		try
		{
			pin_ptr<ILogger^> loggerGard = &logger;

			RedirectToLogger(logger);
			auto coverageData = RunCoverage(command, arguments, workingDir, modulePaths, sourcePaths);
			
			auto currentLibraryPath = GetCurrentLibraryPath();
			auto templatePath = currentLibraryPath / L"Template";
			Exporter::HtmlExporter exporter{ templatePath };
			auto outputFolder = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

			exporter.Export(coverageData, outputFolder);
			auto mainPage = outputFolder / "index.html";
			return ToManagedString(mainPage.wstring());
		}
		catch (const std::exception& e)
		{
			std::string message = "Error: ";
			
			message += e.what();			
			throw gcnew VSPackageException(ToManagedString(message));
		}
	}
}