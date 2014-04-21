#include "stdafx.h"

#include <iterator>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "CppCoverage/FileCoverage.hpp"
#include "Exporter/Html/HtmlFileCoverageExporter.hpp"

namespace fs = boost::filesystem;

namespace ExporterTest
{
	namespace
	{
		//---------------------------------------------------------------------
		class HtmlFileCoverageExporterTest: public ::testing::Test
		{
		public:
			HtmlFileCoverageExporterTest()
			{
				fs::path path = "../TestCoverageConsole/TestCoverageConsole.cpp";

				CppCoverage::FileCoverage fileCoverage{ path };
				Exporter::HtmlFileCoverageExporter exporter;

				fileCoverage.AddLine(11, true);
				fileCoverage.AddLine(12, true);
				fileCoverage.AddLine(13, false);

				std::wstring exportedString = GetExportedString(exporter, fileCoverage);				
				boost::split(lines_, exportedString, boost::is_any_of("\n"));
			}
		
			//-----------------------------------------------------------------
			bool HasBeenExecuted(int lineNumber)
			{				
				return boost::starts_with(lines_[lineNumber - 1],
					Exporter::HtmlFileCoverageExporter::StyleBackgroundColorExecuted);
			}

			//-----------------------------------------------------------------
			bool HasBeenUnexecuted(int lineNumber)
			{
				return boost::starts_with(lines_[lineNumber - 1],
					Exporter::HtmlFileCoverageExporter::StyleBackgroundColorUnexecuted);
			}

		private:
			//-----------------------------------------------------------------
			std::wstring GetExportedString(
				Exporter::HtmlFileCoverageExporter& exporter,
				const CppCoverage::FileCoverage& fileCoverage)
			{
				std::wostringstream ostr;
				exporter.Export(fileCoverage, ostr);

				return ostr.str();
			}

		private:
			std::vector<std::wstring> lines_;
		};
	}

	//---------------------------------------------------------------------
	TEST_F(HtmlFileCoverageExporterTest, ExecutedLine)
	{		
		ASSERT_TRUE(HasBeenExecuted(11));
		ASSERT_TRUE(HasBeenExecuted(12));
	}

	//---------------------------------------------------------------------
	TEST_F(HtmlFileCoverageExporterTest, UnexecutedLine)
	{
		ASSERT_TRUE(HasBeenUnexecuted(13));
	}

	//---------------------------------------------------------------------
	TEST_F(HtmlFileCoverageExporterTest, NotRunnableLine)
	{
		ASSERT_FALSE(HasBeenExecuted(14));
		ASSERT_FALSE(HasBeenUnexecuted(14));
	}
}