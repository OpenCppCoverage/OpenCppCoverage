#include "stdafx.h"
#include "StartInfo.hpp"

#include <boost/filesystem.hpp>

#include "CppCoverageException.hpp"

namespace FS = boost::filesystem;

namespace CppCoverage
{
	namespace
	{
		void CheckFileExists(const std::wstring& filename)
		{			
			if (!FS::exists(filename))
				THROW(L"File " << filename << L" does not exists.");
		}
	}
	//-------------------------------------------------------------------------
	StartInfo::StartInfo(const std::wstring& filename)
		: filename_(filename)
	{
		CheckFileExists(filename);
		AddArguments(filename);
	}

	//-------------------------------------------------------------------------
	void StartInfo::SetWorkingDirectory(const std::wstring* workingDirectory)
	{
		if (workingDirectory)
		{
			CheckFileExists(*workingDirectory);
			workingDirectory_ = *workingDirectory;			
		}
		else
			workingDirectory_.reset();
	}

	//-------------------------------------------------------------------------
	void StartInfo::AddArguments(const std::wstring& argument)
	{
		arguments_.push_back(argument);
	}

	//-------------------------------------------------------------------------
	const std::wstring& StartInfo::GetFilename() const
	{
		return filename_;
	}

	//-------------------------------------------------------------------------
	const std::vector<std::wstring>& StartInfo::GetArguments() const
	{
		return arguments_;
	}

	//-------------------------------------------------------------------------
	const std::wstring* StartInfo::GetWorkingDirectory() const
	{
		if (workingDirectory_)
			return &workingDirectory_.get();
		return nullptr;
	}

	//-------------------------------------------------------------------------
	std::wostream& operator<<(std::wostream& ostr, const StartInfo& startInfo)
	{
		ostr << L"Filename:" << startInfo.filename_ << std::endl;
		ostr << L"Arguments:";
		
		for (const auto& arg : startInfo.arguments_)
			ostr << arg << " ";
		ostr << std::endl;
		ostr << L"Working directory:" << startInfo.workingDirectory_ << std::endl;

		return ostr;
	}
}
