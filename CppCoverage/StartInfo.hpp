#ifndef CPPCOVERAGE_STARTINFO_HEADER_GARD
#define CPPCOVERAGE_STARTINFO_HEADER_GARD

#include <string>
#include <vector>
#include <iosfwd>
#include <boost/optional.hpp>

#include "Export.hpp"

namespace CppCoverage
{
	class DLL StartInfo
	{
	public:
		explicit StartInfo(const std::wstring& filename);
		
		StartInfo(const StartInfo&) = default;
		StartInfo& operator=(const StartInfo&) = default;

		void SetWorkingDirectory(const std::wstring*);
		void AddArguments(const std::wstring&);

		const std::wstring& GetFilename() const;
		const std::vector<std::wstring>& GetArguments() const;
		const std::wstring* GetWorkingDirectory() const;				

		friend std::wostream& operator<<(std::wostream& ostr, const StartInfo&);

	private:
		std::wstring filename_;
		std::vector<std::wstring> arguments_;
		boost::optional<std::wstring> workingDirectory_;
	};
}

#endif
