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

#include "PathMatcher.hpp"
#include <boost/filesystem.hpp>

#include <boost/optional/optional.hpp>
#include <boost/algorithm/string.hpp>

#include <unordered_map>
#include <map>

#include "AmbiguousPathException.hpp"
#include "File.hpp"

namespace fs = boost::filesystem;

namespace FileFilter
{	
	namespace
	{
		//---------------------------------------------------------------------
		fs::path NormalizePath(const fs::path& path)
		{
			fs::path lowerPath = boost::algorithm::to_lower_copy(path.wstring());
			lowerPath.make_preferred();

			return lowerPath;
		}
	}

	//-------------------------------------------------------------------------
	class PathMatcher::IPathMatcherEngine
	{
	public:
		virtual ~IPathMatcherEngine() = default;
		virtual File* Match(const fs::path&) = 0;
		virtual PathCollection GetUnmatchedPaths() const = 0;
	};

	//---------------------------------------------------------------------
	class PathMatcher::PostFixPathMatcherEngine: public PathMatcher::IPathMatcherEngine
	{
	public:	
		//-----------------------------------------------------------------
		explicit PostFixPathMatcherEngine(std::vector<File>&& files)
		{			
			for (auto&& file : files)
			{
				auto path = NormalizePath(file.GetPath());
				auto filename = path.filename().wstring();
				postFixPathByFilename_[filename].emplace_back(std::move(file));
			}			
		}

		//-----------------------------------------------------------------
		File* Match(const fs::path& path) override
		{		
			const auto normalizedPath = NormalizePath(path);
			const auto filenameStr = normalizedPath.filename().wstring();
			auto it = postFixPathByFilename_.find(filenameStr);

			if (it == postFixPathByFilename_.end())
				return nullptr;
						
			for (auto& pathData : it->second)
			{
				auto postFixPath = NormalizePath(pathData.postFixPath_.GetPath());

				if (boost::algorithm::ends_with(normalizedPath.wstring(), postFixPath.wstring()))
				{
					if (pathData.matchedPath_ && 
						!boost::algorithm::equals(pathData.matchedPath_->wstring(), normalizedPath.wstring()))
					{
						throw AmbiguousPathException(postFixPath,
							*pathData.matchedPath_, normalizedPath);
					}
					pathData.matchedPath_ = normalizedPath;
					return &pathData.postFixPath_;
				}
			}

			return nullptr;
		}

		//-----------------------------------------------------------------
		PathCollection GetUnmatchedPaths() const override
		{
			PathCollection paths;

			for (const auto& pair : postFixPathByFilename_)
			{
				const auto& postFixPathCollection = pair.second;
				for (const auto& pathData : postFixPathCollection)
				{
					if (!pathData.matchedPath_)
						paths.push_back(pathData.postFixPath_.GetPath());
				}
			}

			return paths;
		}

	private:
		struct PathData
		{
			explicit PathData(File&& postFixPath) : postFixPath_{ std::move(postFixPath) } {}
			PathData(PathData&& pathData) = default;
			
			File postFixPath_;
			boost::optional<fs::path> matchedPath_;
		};
		
		std::unordered_map<std::wstring, std::vector<PathData>> postFixPathByFilename_;
	};

	//---------------------------------------------------------------------
	class PathMatcher::FullPathMatcherEngine : public PathMatcher::IPathMatcherEngine
	{
	public:
		//-----------------------------------------------------------------
		explicit FullPathMatcherEngine(const fs::path& parentPath, std::vector<File>&& files)
		{
			for (auto& file : files)
			{
				auto fullPath = parentPath / file.GetPath();				
				auto normalizedFullPathStr = NormalizePath(fullPath).wstring();
				pathDataByPath_.emplace(normalizedFullPathStr, PathData{ std::move(file) });
			}
		}

		//-----------------------------------------------------------------
		File* Match(const fs::path& path) override
		{
			auto normalizedPath = NormalizePath(path);
			auto it = pathDataByPath_.find(normalizedPath.wstring());

			if (it == pathDataByPath_.end())
				return nullptr;

			auto& pathData = it->second;
			pathData.haveBeenMarched_ = true;

			return &pathData.file_;
		}

		//-----------------------------------------------------------------
		PathCollection GetUnmatchedPaths() const override
		{
			PathCollection paths;

			for (const auto& pair : pathDataByPath_)
			{
				const auto& pathData = pair.second;
				if (!pathData.haveBeenMarched_)
					paths.push_back(pair.first);
			}

			return paths;
		}

	private:
	
		struct PathData
		{
			explicit PathData(File&& fullPath) :
				file_{ std::move(fullPath) },
				haveBeenMarched_{false} 
			{}

			PathData(PathData&& pathData) = default;

			File file_;
			bool haveBeenMarched_;
		};
		
		std::map<fs::path, PathData> pathDataByPath_;
	};

	//-------------------------------------------------------------------------
	PathMatcher::PathMatcher(
		std::vector<File>&& files,
		const boost::optional<fs::path>& parentPath)
	{
		if (parentPath)
			pathMatcherEngine_ = std::make_unique<FullPathMatcherEngine>(*parentPath, std::move(files));
		else
			pathMatcherEngine_ = std::make_unique<PostFixPathMatcherEngine>(std::move(files));
	}
	
	//-------------------------------------------------------------------------
	PathMatcher::~PathMatcher()
	{
	}

	//-------------------------------------------------------------------------
	File* PathMatcher::Match(const fs::path& path)
	{
		return pathMatcherEngine_->Match(path);		
	}

	//-------------------------------------------------------------------------
	std::vector<fs::path> PathMatcher::GetUnmatchedPaths() const
	{
		return pathMatcherEngine_->GetUnmatchedPaths();
	}
}
