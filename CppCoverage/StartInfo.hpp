#pragma once

#include <string>
#include <vector>
#include <iosfwd>
#include <boost/optional.hpp>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL StartInfo
	{
	public:
		explicit StartInfo(const std::wstring& filename);
		
		StartInfo(const StartInfo&) = default;
		StartInfo& operator=(const StartInfo&) = default;

		void SetWorkingDirectory(const std::wstring*);
		void AddArguments(const std::wstring&);

		const std::wstring& GetFilename() const; // $$ rename to path !!!
		const std::vector<std::wstring>& GetArguments() const;
		const std::wstring* GetWorkingDirectory() const;				

		friend std::wostream& operator<<(std::wostream& ostr, const StartInfo&);

	private:
		std::wstring filename_;
		std::vector<std::wstring> arguments_;
		boost::optional<std::wstring> workingDirectory_;
	};
}


