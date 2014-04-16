#include "stdafx.h"
#include "Options.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	Options::Options(
		const StartInfo& startInfo,
		const Patterns& modulePatterns,
		const Patterns& sourcePatterns)
		: startInfo_{startInfo}
		, modules_{modulePatterns}
		, sources_{sourcePatterns}
		, verboseModeSelected_{false}
	{
	}	

	//-------------------------------------------------------------------------
	const Patterns& Options::GetModulePatterns() const
	{
		return modules_;
	}
	
	//-------------------------------------------------------------------------
	const Patterns& Options::GetSourcePatterns() const
	{
		return sources_;
	}
	
	//-------------------------------------------------------------------------
	const StartInfo& Options::GetStartInfo() const
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
