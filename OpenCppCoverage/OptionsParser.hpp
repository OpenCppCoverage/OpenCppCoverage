#pragma once

#include "OpenCppCoverageExport.hpp"

#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#include <iosfwd>

namespace OpenCppCoverage
{
	class Options;

	class OPEN_CPP_COVERAGE_DLL OptionsParser
	{
	public:
		static const std::string SelectedModulesOption;
		static const std::string ExcludedModulesOption;
		static const std::string SelectedSourcesOption;
		static const std::string ExcludedSourcesOption;
		static const std::string VerboseOption;
		static const std::string VerboseShortOption;
		static const std::string HelpOption;
		static const std::string HelpShortOption;
		static const std::string WorkingDirectoryOption;

	public:
		OptionsParser();

		boost::optional<Options> Parse(int argc, const char** argv) const;
		
		friend OPEN_CPP_COVERAGE_DLL std::wostream& operator<<(std::wostream&, const OptionsParser&);

	private:
		OptionsParser(const OptionsParser&) = delete;
		OptionsParser& operator=(const OptionsParser&) = delete;

		void FillVariableMap(int argc, const char** argv, boost::program_options::variables_map& variables) const;

	private:
		boost::program_options::options_description description_;
	};
}

