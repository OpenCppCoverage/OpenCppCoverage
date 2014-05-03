#pragma once

#include <regex>
#include <string>

#include "CppCoverageExport.hpp"

namespace CppCoverage
{
	class CPPCOVERAGE_DLL Wildcards
	{
	public:
		static const std::vector<wchar_t> EscapedChars;

	public:
		explicit Wildcards(std::wstring, bool isRegexCaseSensitiv = false);
		Wildcards(Wildcards&&);

		bool Match(const std::wstring& str) const;

		friend CPPCOVERAGE_DLL std::wostream& operator<<(std::wostream&, const Wildcards&);

	private:
		Wildcards(const Wildcards&) = delete;
		Wildcards& operator=(const Wildcards&) = delete;

	private:
		std::wregex wildcars_;
		std::wstring originalStr_;
	};
}