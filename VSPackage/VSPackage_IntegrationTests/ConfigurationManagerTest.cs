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

using EnvDTE80;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VSSDK.Tools.VsIdeTesting;
using OpenCppCoverage.VSPackage;
using System.Linq;

namespace VSPackage_IntegrationTests
{
    [TestClass()]
    public class ConfigurationManagerTest
    {
        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void ProjectNotMarkedAsBuild()
        {            
            var configuration = TestHelpers.OpenDefaultSolution(
                TestHelpers.CppConsoleApplication, ConfigurationName.Release, PlatFormName.Win32);            
            var error = GetConfigurationError(configuration);
            Assert.AreEqual(string.Format(
                "The project {0} is marked" + 
                " as not build for the active solution configuration. Please check your " + 
                "solution Configuration Manager.", TestHelpers.CppConsoleApplication), error);                    
        }

        //---------------------------------------------------------------------
        static string GetConfigurationError(SolutionConfiguration2 solutionConfiguration)
        {
            var solution = VsIdeTestHostContext.Dte.Solution;
            var configurationManager = new ConfigurationManager(solutionConfiguration);            
            var project = solution.Projects.Cast<EnvDTE.Project>().First(p => p.UniqueName == TestHelpers.CppConsoleApplication);
            var extendedProject = new ExtendedProject(project, new DynamicVCProject(project.Object));

            try
            {
                configurationManager.GetConfiguration(extendedProject);
                return null;
            }
            catch (VSPackageException e)
            {
                return e.Message;
            }
        }
    }
}
