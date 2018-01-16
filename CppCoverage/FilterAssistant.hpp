// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2018 OpenCppCoverage
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

#pragma once

#include <boost/optional/optional_fwd.hpp>
#include <memory>
#include "CppCoverageExport.hpp"

namespace boost
{
	namespace filesystem
	{
		class path;
	}
}

namespace CppCoverage
{
	class IFileSystem;

	class CPPCOVERAGE_DLL FilterAssistant
	{
	  public:
		explicit FilterAssistant(std::shared_ptr<IFileSystem>);
		~FilterAssistant();

		void OnNewModule(const boost::filesystem::path&, bool isSelected);
		boost::optional<boost::filesystem::path>
		ComputeSuggestedModuleFilter() const;

		void OnNewSourceFile(const boost::filesystem::path&, bool isSelected);
		boost::optional<boost::filesystem::path>
		ComputeSuggestedSourceFileFilter() const;

	  private:
		class SuggestedFilter;

		std::unique_ptr<SuggestedFilter> suggestedModuleFilter_;
		std::unique_ptr<SuggestedFilter> suggestedSourceFileFilter_;
	};
}
