// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
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
#include "CoverageDataSerializer.hpp"

#include "CoverageData.pb.h"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"

#include "../ExporterException.hpp"

#include "Tools/Tool.hpp"

#include "ProtoBuff.hpp"

namespace pb = ProtoBuff;
namespace cov = CppCoverage;

namespace Exporter
{
	namespace
	{
		//---------------------------------------------------------------------
		void InitializeProtoBuffFrom(
			const cov::FileCoverage& file,
			pb::FileCoverage& fileProtoBuff)
		{
			fileProtoBuff.set_path(Tools::ToUtf8String(file.GetPath().wstring()));

			for (const auto& line : file.GetLines())
			{
				auto lineProtoBuff = fileProtoBuff.add_lines();
				
				lineProtoBuff->set_linenumber(line.GetLineNumber());
				lineProtoBuff->set_hasbeenexecuted(line.HasBeenExecuted());
			}
		}

		//---------------------------------------------------------------------
		void InitializeModuleProtoBuffFrom(
			const cov::ModuleCoverage& module,
			pb::ModuleCoverage& moduleProtoBuff)
		{
			moduleProtoBuff.set_path(Tools::ToUtf8String(module.GetPath().wstring()));
			
			for (const auto& file : module.GetFiles())
			{
				auto fileProtoBuff = moduleProtoBuff.add_files();
				InitializeProtoBuffFrom(*file, *fileProtoBuff);
			}
		}

		//---------------------------------------------------------------------
		void FillCoverageDataProtoBuffFrom(
			const cov::CoverageData& coverageData,
			pb::CoverageData& coverageDataProtoBuff)
		{
			coverageDataProtoBuff.set_name(Tools::ToUtf8String(coverageData.GetName()));
			coverageDataProtoBuff.set_exitcode(coverageData.GetExitCode());
			coverageDataProtoBuff.set_modulecount(coverageData.GetModules().size());			
		}

		//---------------------------------------------------------------------
		void WriteMessage(
			const google::protobuf::MessageLite& message, 
			google::protobuf::io::CodedOutputStream& output)
		{
			output.WriteVarint32(message.ByteSize());
			if (!message.SerializeToCodedStream(&output))
				THROW(L"Cannot serialize message to stream");
		}
	}

	//-------------------------------------------------------------------------
	const unsigned int CoverageDataSerializer::FileTypeId = 1351727964; // random number
	
	//-------------------------------------------------------------------------
	void CoverageDataSerializer::Serialize(
		const cov::CoverageData& coverageData,
		const boost::filesystem::path& output) const
	{		
		pb::CoverageData coverageDataProtoBuff;
		Tools::CreateParentFolderIfNeeded(output);

		std::ofstream ofs(output.string(), std::ios::binary);
		if (!ofs)
		{
			throw std::runtime_error(
			    "Cannot write binary export to the file: " + output.string());
		}

		google::protobuf::io::OstreamOutputStream outputStream(&ofs);
		google::protobuf::io::CodedOutputStream codedOutputStream(&outputStream);

		codedOutputStream.WriteVarint32(CoverageDataSerializer::FileTypeId);

		FillCoverageDataProtoBuffFrom(coverageData, coverageDataProtoBuff);
		WriteMessage(coverageDataProtoBuff, codedOutputStream);
				 			
		// Here we serialize manually modules because protobuff's limit.
		// See https://developers.google.com/protocol-buffers/docs/techniques#large-data
		for (const auto& module : coverageData.GetModules())
		{
			pb::ModuleCoverage moduleProtoBuff;
			InitializeModuleProtoBuffFrom(*module, moduleProtoBuff);

			WriteMessage(moduleProtoBuff, codedOutputStream);
		}
	}
}
