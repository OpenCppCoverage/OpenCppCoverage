#ifndef CPPCOVERAGE_MODULECOVERAGE_HEADER_GARD
#define CPPCOVERAGE_MODULECOVERAGE_HEADER_GARD

#include <string>
#include <vector>
#include <memory>

#include "Export.hpp"

namespace CppCoverage
{
	class FileCoverage;

	class DLL ModuleCoverage
	{
	public:
		typedef std::vector<std::unique_ptr<FileCoverage>> T_FileCoverageCollection;

	public:
		explicit ModuleCoverage(const std::wstring& name);
		~ModuleCoverage();

		FileCoverage& AddFile(const std::wstring& filename);

		const std::wstring& GetName() const;
		const T_FileCoverageCollection& GetFiles() const;

	private:
		ModuleCoverage(const ModuleCoverage&) = delete;
		ModuleCoverage& operator=(const ModuleCoverage&) = delete;

	private:
		T_FileCoverageCollection files_;
		std::wstring name_;
	};
}

#endif
