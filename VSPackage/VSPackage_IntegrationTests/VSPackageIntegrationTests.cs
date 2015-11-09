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

using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VisualStudio.VCProjectEngine;
using Microsoft.VSSDK.Tools.VsIdeTesting;
using OpenCppCoverage.VSPackage;
using System;
using System.IO;
using System.Text;

namespace VSPackage_IntegrationTests
{
    [TestClass()]
    public class VSPackageIntegrationTests
    {
        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void EmptySolution()
        {
            var solutionService = TestHelpers.GetService<IVsSolution>();
            solutionService.CloseSolutionElement((uint)__VSSLNSAVEOPTIONS.SLNSAVEOPT_NoSave, null, 0);
            Assert.AreEqual("OpenCppCoverage\n\nCannot get startup projects.", TestHelpers.GetOpenCppCoverageMessage());
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void NotCppStartupProject()
        {
            TestHelpers.OpenDefaultSolution(TestHelpers.CSharpConsoleApplication);
            Assert.AreEqual("OpenCppCoverage\n\nNo C++ startup project found.", TestHelpers.GetOpenCppCoverageMessage());
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void SeveralStartupProjects()
        {
            TestHelpers.OpenDefaultSolution(TestHelpers.CppConsoleApplication, TestHelpers.CppConsoleApplication2);         
            var message = new StringBuilder("OpenCppCoverage\n\nYou cannot run OpenCppCoverage for several projects:\n");
            message.Append(" - " + TestHelpers.CppConsoleApplication + '\n');
            message.Append(" - " + TestHelpers.CppConsoleApplication2);

            Assert.AreEqual(message.ToString(), TestHelpers.GetOpenCppCoverageMessage());
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void ProjectInFolder()
        {
            TestHelpers.OpenDefaultSolution(TestHelpers.ConsoleApplicationInFolder);
            var output = TestHelpers.ExecuteOpenCppCoverageAndReturnOutput(TestHelpers.ConsoleApplicationInFolderName);
            Assert.AreNotEqual("", output);
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void DoesNotCompile()
        {            
            TestHelpers.OpenDefaultSolution(TestHelpers.CppConsoleApplication2);
            Assert.AreEqual("OpenCppCoverage\n\nBuild failed.", TestHelpers.GetOpenCppCoverageMessage());
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void InvalidWorkingDirectory()
        {            
            CheckInvalidSettings(
                (debugSettings, v) => debugSettings.WorkingDirectory = v, 
                debugSettings => debugSettings.WorkingDirectory, 
                "OpenCppCoverage\n\nDebugging working directory \"{0}\" does not exist.");
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void InvalidCommand()
        {
            CheckInvalidSettings(
                (debugSettings, v) => debugSettings.Command = v,
                debugSettings => debugSettings.Command,
                "OpenCppCoverage\n\nDebugging command \"{0}\" does not exist.");
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void CheckCoverageX86()
        {
            TestHelpers.OpenDefaultSolution(TestHelpers.CppConsoleApplication);
            CheckCoverage(ConfigurationName.Debug, PlatFormName.Win32);
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void CheckCoverageX64()
        {
            
            CheckCoverage(ConfigurationName.Debug, PlatFormName.x64);
        }

        //---------------------------------------------------------------------
        void CheckCoverage(
            ConfigurationName configurationName, 
            PlatFormName platformName)
        {            
            var configuration = TestHelpers.OpenDefaultSolution(TestHelpers.CppConsoleApplication, configurationName, platformName);
            
            var debugSettings = SolutionConfigurationHelpers.GetCurrentDebugSettings(TestHelpers.CppConsoleApplication);
            SolutionConfigurationHelpers.CleanSolution();
            debugSettings.CommandArguments = "Test";

            var output = TestHelpers.ExecuteOpenCppCoverageAndReturnOutput(TestHelpers.ApplicationName);
            CheckOutput(output, CoverageRunner.ProjectNameTag, TestHelpers.CppConsoleApplication);
            CheckOutput(output, CoverageRunner.CommandTag, TestHelpers.ApplicationName);
            CheckOutput(output, CoverageRunner.ArgumentTag, debugSettings.CommandArguments);
            CheckOutput(output, CoverageRunner.WorkingDirTag, GetProjectFolder(TestHelpers.CppConsoleApplication));
            CheckOutput(output, CoverageRunner.SelectedFolderTag, GetProjectFolder(TestHelpers.CppConsoleApplication));
            CheckOutput(output, CoverageRunner.SelectedFolderTag, GetProjectFolder(TestHelpers.CppConsoleApplication2));
            CheckOutput(output, CoverageRunner.SelectedModuleTag, TestHelpers.ApplicationName);
            CheckOutput(output, CoverageRunner.SelectedModuleTag, TestHelpers.ApplicationName2);
            CheckOutput(output, "Report was generating at", "");
        }
        
        //---------------------------------------------------------------------
        static string GetProjectFolder(string projectName)
        {
            var rootFolder = TestHelpers.GetIntegrationTestsSolutionFolder();

            return Path.GetDirectoryName(Path.Combine(rootFolder, projectName));
        }

        //---------------------------------------------------------------------
        static void CheckOutput(string output, string lineStartsWith, string textToFound)
        {
            using (var reader = new StringReader(output))
            {
                var line = reader.ReadLine();

                while (line != null)
                {
                    if (line.StartsWith(lineStartsWith) && line.Contains(textToFound))
                        return;
                    line = reader.ReadLine();
                }
            }

            Assert.Fail(string.Format("Cannot found {0} with a starting line :{1}",
                textToFound, lineStartsWith));
        }

        //---------------------------------------------------------------------
        static void CheckInvalidSettings(
            Action<VCDebugSettings, string> setter, 
            Func<VCDebugSettings, string> getter, 
            string expectedMessage)
        {
            TestHelpers.OpenDefaultSolution(TestHelpers.CppConsoleApplication);
            var debugSettings = SolutionConfigurationHelpers.GetCurrentDebugSettings(TestHelpers.CppConsoleApplication);
            const string InvalidValue = "InvalidValue";
            var oldValue = getter(debugSettings);
            try
            {
                setter(debugSettings, InvalidValue);

                var fullExpectedMessage = string.Format(expectedMessage, InvalidValue);
                Assert.AreEqual(fullExpectedMessage, TestHelpers.GetOpenCppCoverageMessage());
            }
            finally
            {
                setter(debugSettings, oldValue);
            }
        }
    }
}
