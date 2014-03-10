#pragma once

#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <map>

#include "LineCoverage.hpp"
#include "CppCoverageExport.hpp"
#include "CoverageRate.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL FileCoverage
	{
	public:
		explicit FileCoverage(const boost::filesystem::path& path);

		void AddLine(unsigned int lineNumber, bool hasBeenExecuted);
		void ComputeCoverageRate();
		const CoverageRate& GetCoverageRate() const;

		const boost::filesystem::path& GetPath() const;
		const LineCoverage* operator[](unsigned int line) const;

	private:
		FileCoverage(const FileCoverage&) = delete;
		FileCoverage& operator=(const FileCoverage&) = delete;
	
	private:
		boost::filesystem::path path_;
		std::map<unsigned int, LineCoverage> lines_;
		CoverageRate coverageRate_;
	};
}


