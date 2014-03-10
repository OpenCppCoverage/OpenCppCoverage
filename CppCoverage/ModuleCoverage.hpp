#pragma once

#include <vector>
#include <memory>

#include <boost/filesystem.hpp>

#include "CppCoverageExport.hpp"
#include "CoverageRate.hpp"

namespace CppCoverage
{
	class FileCoverage;

	class CPPCOVERAGE_DLL ModuleCoverage
	{
	public:
		typedef std::vector<std::unique_ptr<FileCoverage>> T_FileCoverageCollection;

	public:
		explicit ModuleCoverage(const boost::filesystem::path& path);
		~ModuleCoverage();

		FileCoverage& AddFile(const boost::filesystem::path& filename);
		void ComputeCoverageRate();
		const CoverageRate& GetCoverageRate() const;

		const boost::filesystem::path& GetPath() const;
		const T_FileCoverageCollection& GetFiles() const;

	private:
		ModuleCoverage(const ModuleCoverage&) = delete;
		ModuleCoverage& operator=(const ModuleCoverage&) = delete;

	private:
		T_FileCoverageCollection files_;
		boost::filesystem::path path_;
		CoverageRate coverageRate_;
	};
}


