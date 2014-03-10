#pragma once

#include <string>
#include <vector>

#include <memory>
#include <unordered_map>

#include "CoverageData.hpp"
#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class FileCoverage;

	class CPPCOVERAGE_DLL ExecutedAddressManager
	{
	public:
		ExecutedAddressManager();
		~ExecutedAddressManager();

		void SetCurrentModule(const std::wstring& moduleName);
		bool RegisterAddress(void* address, const std::wstring& filename, unsigned int line, unsigned char instruction);
		unsigned  char MarkAddressAsExecuted(void* address);

		CoverageData CreateCoverageData(const std::wstring& name) const;

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


