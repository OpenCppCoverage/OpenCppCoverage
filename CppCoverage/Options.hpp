#pragma once

#include "CppCoverageExport.hpp"
#include "Patterns.hpp"
#include "StartInfo.hpp"

namespace CppCoverage
{
	class Patterns;
	
	class CPPCOVERAGE_DLL Options
	{
	public:
		Options(
			const StartInfo& startInfo,
			const Patterns& modulePatterns,
			const Patterns& sourcePatterns);

		Options(const Options&) = default;
		
		const Patterns& GetModulePatterns() const;			
		const Patterns& GetSourcePatterns() const;
		const StartInfo& GetStartInfo() const;

		void SetVerboseModeSelected();
		bool IsVerboseModeSelected() const;
				
	private:
		Options& operator=(Options&&) = delete;

	private:
		StartInfo startInfo_;
		Patterns modules_;
		Patterns sources_;
		bool verboseModeSelected_;
	};
}