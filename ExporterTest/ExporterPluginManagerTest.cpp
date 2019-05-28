// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2019 OpenCppCoverage
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

#include "TestHelper/TemporaryPath.hpp"
#include "TestHelper/Tools.hpp"

#include "Plugin/Exporter/IExportPlugin.hpp"

#include "Exporter/Plugin/IPluginLoader.hpp"
#include "Exporter/Plugin/LoadedPlugin.hpp"
#include "Exporter/Plugin/ExporterPluginManager.hpp"

#include "Plugin/Exporter/CoverageData.hpp"

#include "Tools/Tool.hpp"

using testing::_;

namespace ExporterTest
{
	namespace
	{
		//---------------------------------------------------------------------
		class PluginLoaderMock
		    : public Exporter::IPluginLoader<Plugin::IExportPlugin>
		{
		  public:
			MOCK_CONST_METHOD2(
			    TryLoadPlugin,
			    std::unique_ptr<Exporter::LoadedPlugin<Plugin::IExportPlugin>>(
			        const std::filesystem::path& pluginPath,
			        const std::string& pluginFactoryFctName));
		};

		//---------------------------------------------------------------------
		class ExportPluginMock : public Plugin::IExportPlugin
		{
		  public:
			MOCK_METHOD2(Export,
			             std::optional<std::filesystem::path>(
			                 const Plugin::CoverageData&,
			                 const std::optional<std::wstring>& argument));

			MOCK_METHOD1(CheckArgument,
			             void(const std::optional<std::wstring>&));
			MOCK_METHOD0(GetArgumentHelpDescription, std::wstring());
			MOCK_CONST_METHOD0(GetExportPluginVersion, int());
		};
	}

	//-------------------------------------------------------------------------
	class ExporterPluginManagerTest : public ::testing::Test
	{
	  public:
		//---------------------------------------------------------------------
		ExporterPluginManagerTest()
		    : pluginFolder_(TestHelper::TemporaryPathOption::CreateAsFolder)
		{
			pluginPath_ = pluginFolder_.GetPath() / "Plugin.dll";
			pluginName_ = pluginPath_.stem();
			TestHelper::CreateEmptyFile(pluginPath_);
			TestHelper::CreateEmptyFile(pluginFolder_.GetPath() / "File.txt");
		}

		//---------------------------------------------------------------------
		std::unique_ptr<Exporter::ExporterPluginManager>
		CreateManager(std::unique_ptr<ExportPluginMock> exportPlugin)
		{
			PluginLoaderMock pluginLoader;

			EXPECT_CALL(pluginLoader, TryLoadPlugin(pluginPath_, _))
			    .WillOnce(testing::Invoke([&](const auto& p, const auto&) {
				    EXPECT_EQ(pluginPath_, p);
				    auto plugin = std::make_unique<
				        Exporter::LoadedPlugin<Plugin::IExportPlugin>>(nullptr);
				    plugin->Set(std::move(exportPlugin));

				    return plugin;
			    }));
			return CreateManager(pluginLoader);
		}

		//---------------------------------------------------------------------
		std::unique_ptr<Exporter::ExporterPluginManager>
		CreateManager(PluginLoaderMock& pluginLoader)
		{
			return std::make_unique<Exporter::ExporterPluginManager>(
			    pluginLoader, std::filesystem::path{pluginFolder_.GetPath()});
		}

		//---------------------------------------------------------------------
		std::unique_ptr<ExportPluginMock> CreateExportPluginMock(
		    int pluginVersion = Plugin::CurrentExportPluginVersion) const
		{
			auto exportPlugin = std::make_unique<ExportPluginMock>();

			EXPECT_CALL(*exportPlugin, GetExportPluginVersion())
			    .WillOnce(testing::Return(pluginVersion));

			return exportPlugin;
		}

		std::filesystem::path pluginPath_;
		std::wstring pluginName_;

	  private:
		TestHelper::TemporaryPath pluginFolder_;
	};

	//-------------------------------------------------------------------------
	TEST_F(ExporterPluginManagerTest, CreateExportPluginDescriptions)
	{
		auto exportPlugin = CreateExportPluginMock();

		const std::optional<std::wstring> argument = L"argument";
		const std::wstring description = L"description";

		EXPECT_CALL(*exportPlugin, GetArgumentHelpDescription())
		    .WillOnce(testing::Return(description));
		EXPECT_CALL(*exportPlugin, CheckArgument(argument));

		auto pluginManager = CreateManager(std::move(exportPlugin));
		auto pluginDescriptions =
		    pluginManager->CreateExportPluginDescriptions();

		ASSERT_EQ(1, pluginDescriptions.size());
		const auto& pluginDescription = pluginDescriptions[0];

		ASSERT_EQ(description, pluginDescription.GetParameterDescription());
		ASSERT_EQ(pluginName_, pluginDescription.GetPluginName());
		pluginDescription.CheckArgument(argument);

		pluginManager.reset();
		ASSERT_THROW(pluginDescription.CheckArgument(argument),
		             Exporter::ExporterException);
	}

	//-------------------------------------------------------------------------
	TEST_F(ExporterPluginManagerTest, InvalidVersion)
	{
		auto exportPlugin =
		    CreateExportPluginMock(Plugin::CurrentExportPluginVersion);
		ASSERT_NO_THROW(CreateManager(std::move(exportPlugin)));

		exportPlugin =
		    CreateExportPluginMock(Plugin::CurrentExportPluginVersion + 1);
		ASSERT_THROW(CreateManager(std::move(exportPlugin)),
		             std::runtime_error);
	}

	//-------------------------------------------------------------------------
	TEST_F(ExporterPluginManagerTest, TryLoadPluginFailure)
	{
		PluginLoaderMock pluginLoader;
		const auto errorMessage = "errorMessage";

		EXPECT_CALL(pluginLoader, TryLoadPlugin(pluginPath_, _))
		    .WillOnce(testing::Invoke([&](const auto&, const auto&) {
			    throw 42;
			    return nullptr;
		    }))
		    .WillOnce(testing::Invoke([&](const auto&, const auto&) {
			    throw std::runtime_error(errorMessage);
			    return nullptr;
		    }));

		ASSERT_THROW(CreateManager(pluginLoader), std::runtime_error);

		TestHelper::AssertThrow<std::runtime_error>(
		    [&]() { CreateManager(pluginLoader); },
		    [&](const auto& e) {
			    EXPECT_THAT(e.what(), testing::HasSubstr(errorMessage));
		    });
	}

	//-------------------------------------------------------------------------
	TEST_F(ExporterPluginManagerTest, Export)
	{
		auto exportPlugin = CreateExportPluginMock();
		const std::optional<std::wstring> argument = L"argument";

		EXPECT_CALL(*exportPlugin, Export(_, argument));

		auto pluginManager = CreateManager(std::move(exportPlugin));
		Plugin::CoverageData coverageData{L"", 0};

		pluginManager->Export(pluginName_, coverageData, argument);
	}

	//-------------------------------------------------------------------------
	TEST_F(ExporterPluginManagerTest, InvalidExport)
	{
		auto exportPlugin = CreateExportPluginMock();
		const auto errorMessage = "errorMessage";

		EXPECT_CALL(*exportPlugin, Export(_, _))
		    .WillOnce(testing::Invoke([](const auto&, const auto&) {
			    throw 42;
			    return std::nullopt;
		    }))
		    .WillOnce(testing::Invoke([&](const auto&, const auto&) {
			    throw std::runtime_error(errorMessage);
			    return std::nullopt;
		    }));

		auto pluginManager = CreateManager(std::move(exportPlugin));
		Plugin::CoverageData coverageData{L"", 0};

		ASSERT_THROW(pluginManager->Export(pluginName_, coverageData, L""),
		             std::runtime_error);

		TestHelper::AssertThrow<std::runtime_error>(
		    [&]() { pluginManager->Export(pluginName_, coverageData, L""); },
		    [&](const auto& e) {
			    EXPECT_THAT(e.what(), testing::HasSubstr(errorMessage));
		    });

		ASSERT_THROW(
		    pluginManager->Export(L"InvalidPluginName", coverageData, L""),
		    Exporter::ExporterException);
	}
}
