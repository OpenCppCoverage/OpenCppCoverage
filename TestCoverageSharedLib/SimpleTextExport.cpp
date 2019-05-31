#include "stdafx.h"

#include "Plugin/Exporter/IExportPlugin.hpp"
#include "Plugin/Exporter/CoverageData.hpp"
#include "Plugin/Exporter/ModuleCoverage.hpp"
#include "Plugin/Exporter/FileCoverage.hpp"
#include "Plugin/Exporter/LineCoverage.hpp"
#include "Plugin/OptionsParserException.hpp"

#include <filesystem>
#include <fstream>

// This class is used by ImportExportTest.ExportPlugin
class SimpleTextExport : public Plugin::IExportPlugin
{
  public:
	//-------------------------------------------------------------------------
	std::optional<std::filesystem::path>
	Export(const Plugin::CoverageData& coverageData,
	       const std::optional<std::wstring>& argument) override
	{
		std::filesystem::path output = argument ? *argument : L"SimpleText.txt";
		std::wofstream ofs{output};

		if (!ofs)
			throw std::runtime_error(
			    "Cannot create the output file for SimpleExport");

		for (const auto& mod : coverageData.GetModules())
		{
			ofs << mod->GetPath().filename().wstring() << std::endl;
			for (const auto& file : mod->GetFiles())
			{
				const auto& lines = file->GetLines();
				auto coveredCount = std::count_if(
				    lines.begin(), lines.end(), [](const auto& line) {
					    return line.HasBeenExecuted();
				    });
				ofs << '\t' << file->GetPath().filename().wstring() << "  ";
				ofs << "Lines covered: " << coveredCount
				    << " Total: " << lines.size() << std::endl;
			}
		}
		return output;
	}

	//-------------------------------------------------------------------------
	void CheckArgument(const std::optional<std::wstring>& argument)
	{
		// Try to check if the argument is a file.
		if (argument && !std::filesystem::path{*argument}.has_filename())
			throw Plugin::OptionsParserException(
			    "Invalid argument for SimpleText export.");
	}

	//-------------------------------------------------------------------------
	std::wstring GetArgumentHelpDescription()
	{
		return L"output file (optional)";
	}

	//-------------------------------------------------------------------------
	int GetExportPluginVersion() const override
	{
		return Plugin::CurrentExportPluginVersion;
	}
};

extern "C"
{
	//-------------------------------------------------------------------------
	__declspec(dllexport) Plugin::IExportPlugin* CreatePlugin()
	{
		return new SimpleTextExport();
	}
}
