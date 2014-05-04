#pragma once

#include "OpenCppCoverageExport.hpp"
#include "CppCoverage/Patterns.hpp"
#include "CppCoverage/StartInfo.hpp"

namespace OpenCppCoverage
{
	class Patterns;
	
	class OPEN_CPP_COVERAGE_DLL Options
	{
	public:
		Options(
			const CppCoverage::StartInfo& startInfo,
			const CppCoverage::Patterns& modulePatterns,
			const CppCoverage::Patterns& sourcePatterns);

		Options(const Options&) = default;
		
		const CppCoverage::Patterns& GetModulePatterns() const;
		const CppCoverage::Patterns& GetSourcePatterns() const;
		const CppCoverage::StartInfo& GetStartInfo() const;

		void SetVerboseModeSelected();
		bool IsVerboseModeSelected() const;
			
		friend OPEN_CPP_COVERAGE_DLL std::wostream& operator<<(std::wostream&, const Options&);

	private:
		Options(const Options&&) = delete;
		Options& operator=(Options&&) = delete;

	private:
		CppCoverage::StartInfo startInfo_;
		CppCoverage::Patterns modules_;
		CppCoverage::Patterns sources_;
		bool verboseModeSelected_;
	};
}