#pragma once

#include <string>
#include <vector>
#include <iosfwd>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL StartInfo
	{
	public:
		explicit StartInfo(const boost::filesystem::path&);
		
		StartInfo(StartInfo&&);

		StartInfo(const StartInfo&) = default;		
		StartInfo& operator=(const StartInfo&) = default;

		void SetWorkingDirectory(const boost::filesystem::path&);
		void AddArguments(const std::wstring&);

		const boost::filesystem::path& GetPath() const;
		const std::vector<std::wstring>& GetArguments() const;
		const boost::filesystem::path* GetWorkingDirectory() const;

		friend CPPCOVERAGE_DLL std::wostream& operator<<(std::wostream& ostr, const StartInfo&);

	private:
		boost::filesystem::path path_;
		std::vector<std::wstring> arguments_;
		boost::optional<boost::filesystem::path> workingDirectory_;
	};
}


