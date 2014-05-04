#include "stdafx.h"
#include "Options.hpp"

namespace cov = CppCoverage;

namespace OpenCppCoverage
{
	//-------------------------------------------------------------------------
	Options::Options(
		const cov::StartInfo& startInfo,
		const cov::Patterns& modulePatterns,
		const cov::Patterns& sourcePatterns)
		: startInfo_{startInfo}
		, modules_{modulePatterns}
		, sources_{sourcePatterns}
		, verboseModeSelected_{false}
	{
	}	

	//-------------------------------------------------------------------------
	const cov::Patterns& Options::GetModulePatterns() const
	{
		return modules_;
	}
	
	//-------------------------------------------------------------------------
	const cov::Patterns& Options::GetSourcePatterns() const
	{
		return sources_;
	}
	
	//-------------------------------------------------------------------------
	const cov::StartInfo& Options::GetStartInfo() const
	{
		return startInfo_;
	}

	//-------------------------------------------------------------------------
	void Options::SetVerboseModeSelected()
	{
		verboseModeSelected_ = true;
	}

	//-------------------------------------------------------------------------
	bool Options::IsVerboseModeSelected() const
	{
		return verboseModeSelected_;
	}

	//-------------------------------------------------------------------------
	std::wostream& operator<<(std::wostream& ostr, const Options& options)
	{
		ostr << options.startInfo_ << std::endl;
		ostr << L"Modules: " << options.modules_ << std::endl;
		ostr << L"Sources: " << options.sources_ << std::endl;
		ostr << L"Verbose mode: " << options.verboseModeSelected_ << std::endl;

		return ostr;
	}
}
