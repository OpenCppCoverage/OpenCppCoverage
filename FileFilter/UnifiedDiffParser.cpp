// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "UnifiedDiffParser.hpp"

#include <sstream>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>

#include "File.hpp"
#include "UnifiedDiffParserException.hpp"

namespace FileFilter
{
	//---------------------------------------------------------------------
	struct UnifiedDiffParser::HunksDifferences
	{
		int startFrom;
		int countFrom;
		int startTo;
		int countTo;
	};

	//---------------------------------------------------------------------
	struct UnifiedDiffParser::Stream
	{
		Stream(std::wistream& istr)
			: istr_{ istr }
			, currentLine_{ 0 }
			, lastLineRead_{ 0 }
		{
		}

		//---------------------------------------------------------------------
		std::wistream& GetLine(std::wstring& line)
		{
			std::getline(istr_, line);
			++currentLine_;
			lastLineRead_ = line;
			return istr_;
		}

		std::wistream& istr_;
		int currentLine_;
		std::wstring lastLineRead_;
	};

	//-------------------------------------------------------------------------
	std::vector<File> UnifiedDiffParser::Parse(std::wistream& istr) const
	{
		std::wstring line;
		std::vector<File> files;
		Stream stream(istr);
		
		while (stream.GetLine(line))
		{
			if (boost::algorithm::starts_with(line, "---"))
				files.emplace_back(ExtractTargetFile(stream));
			else if (boost::algorithm::starts_with(line, "@@"))
			{
				if (files.empty())
					ThrowError(stream, UnifiedDiffParserException::ErrorNoFilenameBeforeHunks);
				auto updatedLines = ExtractUpdatedLines(stream, line);
				files.back().AddSelectedLines(updatedLines);
			}
		}

		return files;
	}
		
	//---------------------------------------------------------------------
	boost::filesystem::path UnifiedDiffParser::ExtractTargetFile(Stream& stream) const
	{
		std::wstring line;		
		if (!stream.GetLine(line))
			ThrowError(stream, UnifiedDiffParserException::ErrorCannotReadLine);
		
		if (!boost::algorithm::starts_with(line, UnifiedDiffParserException::FromFilePrefix))
			ThrowError(stream, UnifiedDiffParserException::ErrorExpectFromFilePrefix);

		const auto startIndex = UnifiedDiffParserException::FromFilePrefix.size();
		const auto endIndex = line.find('\t');
		
		if (endIndex != std::string::npos)
			return line.substr(startIndex, endIndex - startIndex);
		return line.substr(startIndex);
	}

	//-------------------------------------------------------------------------
	UnifiedDiffParser::HunksDifferences 
		UnifiedDiffParser::ExtractHunksDifferences(
			const Stream& stream, 
			const std::wstring& hunksDifferencesLine) const
	{
		std::vector<std::wstring> values;
		boost::algorithm::split(values, hunksDifferencesLine, boost::algorithm::is_any_of(" ,"));
		
		try
		{
			HunksDifferences hunksDifferences;
			hunksDifferences.startFrom = std::stoi(values.at(1));
			hunksDifferences.countFrom = std::stoi(values.at(2));
			hunksDifferences.startTo = std::stoi(values.at(3));
			hunksDifferences.countTo = std::stoi(values.at(4));

			return hunksDifferences;
		}
		catch (std::exception&)
		{
			ThrowError(stream, UnifiedDiffParserException::ErrorInvalidHunks);
		}
		return{};
	}
	
	//-------------------------------------------------------------------------
	std::vector<int> UnifiedDiffParser::ExtractUpdatedLines(		
		Stream& stream,
		const std::wstring& hunksDifferencesLine) const
	{
		HunksDifferences hunksDifferences = ExtractHunksDifferences(stream, hunksDifferencesLine);

		std::wstring lineStr;
		int currentLine = hunksDifferences.startTo;
		const int endLine = hunksDifferences.startTo + hunksDifferences.countTo;
		std::vector<int> updatedLines;
		while (currentLine < endLine && stream.GetLine(lineStr))
		{
			if (!boost::algorithm::starts_with(lineStr, "-"))
			{
				if (boost::algorithm::starts_with(lineStr, "+"))
					updatedLines.push_back(currentLine);
				++currentLine;
			}
		}

		if (currentLine != endLine)
			ThrowError(stream, UnifiedDiffParserException::ErrorContextHunks);
		return updatedLines;
	}
	
	//-------------------------------------------------------------------------
	void UnifiedDiffParser::ThrowError(const Stream& stream, const std::wstring& message) const
	{
		std::wostringstream ostr;

		ostr << L"Error line " << stream.currentLine_ << L": " << stream.lastLineRead_ << std::endl;
		ostr << message;
		throw UnifiedDiffParserException(ostr.str());
	}
}
