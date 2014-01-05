#ifndef CPPCOVERAGE_FILECOVERAGE_HEADER_GARD
#define CPPCOVERAGE_FILECOVERAGE_HEADER_GARD

#include <string>

#include <boost/optional.hpp>
#include <map>

#include "LineCoverage.hpp"
#include "Export.hpp"

namespace CppCoverage
{
	class DLL FileCoverage
	{
	public:
		explicit FileCoverage(const std::wstring& filename);

		void AddLine(unsigned int lineNumber, bool hasBeenExecuted);

		const std::wstring& GetFilename() const;
		const LineCoverage* operator[](unsigned int line) const;

	private:
		FileCoverage(const FileCoverage&) = delete;
		FileCoverage& operator=(const FileCoverage&) = delete;
	
	private:
		std::wstring filename_;
		std::map<unsigned int, LineCoverage> lines_;
	};
}

#endif
