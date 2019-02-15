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

#include "stdafx.h"
#include "FilterAssistant.hpp"
#include "IFileSystem.hpp"

#include <ctime>
#include <boost/optional/optional.hpp>
#include <boost/filesystem/path.hpp>
#include <memory>
#include "ProgramOptions.hpp"
#include "Tools/Tool.hpp"

namespace CppCoverage
{
	namespace
	{
		//-------------------------------------------------------------------------
		std::wstring
		GetAdviceMessage(const std::wstring& noFileSelectedMessage,
		                 const std::string& selectedOption,
		                 const std::string& excludedOption,
		                 const boost::filesystem::path& suggestedFilter)
		{
			auto selectOptionFlag = L"--" + Tools::LocalToWString(selectedOption);
			auto excludedOptionFlag =
			    L"--" + Tools::LocalToWString(excludedOption);

			return noFileSelectedMessage + L" Please check the values of " +
			       selectOptionFlag + L" and " + excludedOptionFlag +
			       L".\n"
			       L"You can try to remove all " +
			       selectOptionFlag + L" and " + excludedOptionFlag +
			       L" and use " + selectOptionFlag + L'=' +
			       suggestedFilter.wstring() + L" instead.";
		}
	}

	//-------------------------------------------------------------------------
	class FilterAssistant::SuggestedFilter
	{
	  public:
		//---------------------------------------------------------------------
		explicit SuggestedFilter(std::shared_ptr<IFileSystem> fileSystem)
		    : fileSystem_{std::move(fileSystem)}
		{
		}

		//---------------------------------------------------------------------
		void OnNewFile(const boost::filesystem::path& file, bool isSelected)
		{
			if (isSelected)
			{
				foundFile_ = true;
				files_.clear();
			}

			if (!foundFile_)
				files_.push_back(file);
		}

		//-------------------------------------------------------------------------
		boost::optional<boost::filesystem::path> ComputeSuggestedFilter() const
		{
			if (foundFile_)
				return boost::none;

			boost::optional<boost::filesystem::path> suggestedFilter;
			std::time_t newest_modification_time = 0;
			for (const auto& file : files_)
			{
				auto time = fileSystem_->GetLastWriteTime(file);
				if (!suggestedFilter ||
				    (time && *time > newest_modification_time))
				{
					suggestedFilter = file.parent_path();
					if (time)
						newest_modification_time = *time;
				}
			}

			return suggestedFilter;
		}

		bool foundFile_ = false;

	  private:
		std::shared_ptr<IFileSystem> fileSystem_;
		std::vector<boost::filesystem::path> files_;
	};

	//-------------------------------------------------------------------------
	const std::wstring FilterAssistant::NoModulesSelectedMsg =
	    L"No modules were selected.";
	const std::wstring FilterAssistant::NoSourceFilesSelectedMsg =
	    L"No source files were selected.";

	//-------------------------------------------------------------------------
	FilterAssistant::FilterAssistant(std::shared_ptr<IFileSystem> fileSystem)
	    : suggestedModuleFilter_{std::make_unique<SuggestedFilter>(fileSystem)},
	      suggestedSourceFileFilter_{std::make_unique<SuggestedFilter>(fileSystem)}
	{
	}

	//-------------------------------------------------------------------------
	FilterAssistant::~FilterAssistant() = default;

	//-------------------------------------------------------------------------
	void FilterAssistant::OnNewModule(const boost::filesystem::path& module,
	                                  bool isSelected)
	{
		suggestedModuleFilter_->OnNewFile(module, isSelected);
	}

	//-------------------------------------------------------------------------
	boost::optional<boost::filesystem::path>
	FilterAssistant::ComputeSuggestedModuleFilter() const
	{
		return suggestedModuleFilter_->ComputeSuggestedFilter();
	}

	//-------------------------------------------------------------------------
	void
	FilterAssistant::OnNewSourceFile(const boost::filesystem::path& sourceFile,
	                              bool isSelected)
	{
		suggestedSourceFileFilter_->OnNewFile(sourceFile, isSelected);
	}

	//-------------------------------------------------------------------------
	boost::optional<boost::filesystem::path>
	FilterAssistant::ComputeSuggestedSourceFileFilter() const
	{
		return suggestedSourceFileFilter_->ComputeSuggestedFilter();
	}

	//-------------------------------------------------------------------------
	boost::optional<std::wstring> FilterAssistant::GetAdviceMessage() const
	{
		auto suggestedModule = ComputeSuggestedModuleFilter();
		if (suggestedModule)
		{
			return CppCoverage::GetAdviceMessage(
			    FilterAssistant::NoModulesSelectedMsg,
			    ProgramOptions::SelectedModulesOption,
			    ProgramOptions::ExcludedModulesOption,
			    *suggestedModule);
		}
		if (!suggestedModuleFilter_->foundFile_)
		{
			return std::wstring{
			    L"No modules with PDB were found. Please check your PDB "
			    L"files."};
		}

		auto suggestedSourceFile = ComputeSuggestedSourceFileFilter();
		if (suggestedSourceFile)
		{
			return CppCoverage::GetAdviceMessage(
			    FilterAssistant::NoSourceFilesSelectedMsg,
			    ProgramOptions::SelectedSourcesOption,
			    ProgramOptions::ExcludedSourcesOption,
			    *suggestedSourceFile);
		}

		return boost::none;
	}
}
