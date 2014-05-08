#include "stdafx.h"
#include "HtmlFile.hpp"

namespace Exporter
{
	//-------------------------------------------------------------------------
	HtmlFile::HtmlFile(
		boost::filesystem::path absolutePath,
		boost::filesystem::path relativeLinkPath)
		: absolutePath_(absolutePath)
		, relativeLinkPath_(relativeLinkPath)
	{
	}

	//-------------------------------------------------------------------------
	const boost::filesystem::path& HtmlFile::GetAbsolutePath() const
	{
		return absolutePath_;
	}

	//-------------------------------------------------------------------------
	const boost::filesystem::path& HtmlFile::GetRelativeLinkPath() const
	{
		return relativeLinkPath_;
	}
}
