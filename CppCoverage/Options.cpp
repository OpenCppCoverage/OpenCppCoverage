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
/*	Options::Options(const Options& options)
		: modules_(options.modules_)
		, sources_(options.sources_) // $$ todo
		, verboseModeSelected_(options.verboseModeSelected_)
	{
	}*/

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
}
