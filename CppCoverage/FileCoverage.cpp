#include "stdafx.h"
#include "FileCoverage.hpp"

#include "CppCoverageException.hpp"

namespace CppCoverage
{
	//-------------------------------------------------------------------------
	FileCoverage::FileCoverage(const std::wstring& filename)
		: filename_(filename)
	{
	}

	//-------------------------------------------------------------------------
	void FileCoverage::AddLine(unsigned int lineNumber, bool hasBeenExecuted)
	{
		LineCoverage line{lineNumber, hasBeenExecuted};

		if (!lines_.insert(std::make_pair(lineNumber, line)).second)
			THROW("Line " << lineNumber << " already exists in " << filename_);
	}

	//-------------------------------------------------------------------------
	const std::wstring& FileCoverage::GetFilename() const
	{
		return filename_;
	}

	//-------------------------------------------------------------------------
	const LineCoverage* FileCoverage::operator[](unsigned int line) const
	{
		auto it = lines_.find(line);

		if (it == lines_.end())
			return 0;

		return &it->second;
	}
}
