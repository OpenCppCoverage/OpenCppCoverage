#pragma once

#include <boost/filesystem.hpp>

#include "../ExporterExport.hpp"

namespace Exporter
{
	class EXPORTER_DLL HtmlFile
	{
	public:
		HtmlFile(
			boost::filesystem::path absolutePath,
			boost::filesystem::path relativeLinkPath);
		HtmlFile(const HtmlFile&) = default;

		const boost::filesystem::path& GetAbsolutePath() const;
		const boost::filesystem::path& GetRelativeLinkPath() const;

	private:		
		HtmlFile& operator=(const HtmlFile&) = delete;

		boost::filesystem::path absolutePath_;
		boost::filesystem::path relativeLinkPath_;
	};
}

