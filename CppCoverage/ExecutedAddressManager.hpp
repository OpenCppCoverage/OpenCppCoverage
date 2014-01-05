#ifndef CPPCOVERAGE_EXECUTEDADDRESSMANAGER_HEADER_GARD
#define CPPCOVERAGE_EXECUTEDADDRESSMANAGER_HEADER_GARD

#include <string>
#include <vector>

#include <memory>
#include <unordered_map>

#include "CoverageData.hpp"
#include "Export.hpp"

namespace CppCoverage
{
	class FileCoverage;

	class DLL ExecutedAddressManager
	{
	public:
		ExecutedAddressManager();
		~ExecutedAddressManager();

		void SetCurrentModule(const std::wstring& moduleName);
		void AddAddress(void* address, const std::wstring& filename, unsigned int line, char instruction);
		char MarkAddressAsExecuted(void* address);

		CoverageData CreateCoverageData() const;

	private:
		struct Module;
		struct File;
		struct Line;

	private:
		ExecutedAddressManager(const ExecutedAddressManager&) = delete;
		ExecutedAddressManager& operator=(const ExecutedAddressManager&) = delete;

		Module& GetCurrentModule();
		void AddFileCoverageInfo(const File& fileData, FileCoverage& fileCoverage) const;
	
	private:
		std::vector<std::unique_ptr<Module>> modules_;
		std::unordered_map<void*, Line*> addressLineMap_;
	};
}

#endif
