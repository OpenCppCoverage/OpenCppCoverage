#ifndef CPPCOVERAGE_MODULECOVERAGE_HEADER_GARD
#define CPPCOVERAGE_MODULECOVERAGE_HEADER_GARD

#include <vector>
#include <memory>

#include <boost/filesystem.hpp>

#include "Export.hpp"

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

		const boost::filesystem::path& GetPath() const;
		const T_FileCoverageCollection& GetFiles() const;

	private:
		ModuleCoverage(const ModuleCoverage&) = delete;
		ModuleCoverage& operator=(const ModuleCoverage&) = delete;

	private:
		T_FileCoverageCollection files_;
		boost::filesystem::path path_;
	};
}

#endif
