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

using EnvDTE;
using EnvDTE80;
using Microsoft.VisualStudio.VCProjectEngine;
using Moq;
using System.Collections.Generic;

namespace VSPackage_UnitTests
{
    public class DteMockBuilder
    {
        static readonly string projectName = "ProjectName";
        static readonly string configurationName = "ConfigurationName";
        static readonly string platformName = "PlatFormName";

        //---------------------------------------------------------------------
        public DteMockBuilder()
        {
            this.VcFiles = new List<VCFile>();
        }

        //---------------------------------------------------------------------
        public Mock<Solution2> BuildSolutionMock()
        {
            var solutionMock = new Mock<Solution2>();
            
            solutionMock.Setup(s => s.Projects)
                .Returns(BuildProjectMock());
            solutionMock.Setup(s => s.SolutionBuild)
                .Returns(BuildSolutionBuildMock());
            return solutionMock;
        }

        //---------------------------------------------------------------------
        public List<VCFile> VcFiles { get; set; }    
        
        //---------------------------------------------------------------------
        Projects BuildProjectMock()
        {
            var vcProject = new Mock<VCProject>();
            var configurations = new List<VCConfiguration> { BuildConfiguration() };
            vcProject.Setup(p => p.Configurations).Returns(configurations);
            vcProject.Setup(p => p.Files).Returns(this.VcFiles);
            vcProject.SetupGet(p => p.Kind).Returns("VCProject");

            var project = new Mock<Project>();
            project.Setup(p => p.UniqueName).Returns(projectName);
            project.Setup(p => p.Object).Returns(vcProject.Object);
            project.SetupGet(p => p.Kind).Returns("");

            var projects = new Mock<Projects>();
            var projectCollection = new List<Project> { project.Object };                        
            projects.Setup(p => p.GetEnumerator()).Returns(projectCollection.GetEnumerator());            
                        
            return projects.Object;
        }

        //---------------------------------------------------------------------
        VCConfiguration BuildConfiguration()
        {
            var debugSettings = new Mock<VCDebugSettings>();
            debugSettings.Setup(d => d.WorkingDirectory).Returns("");
            debugSettings.Setup(d => d.CommandArguments).Returns("");
            debugSettings.Setup(d => d.Command).Returns("");

            var configuration = new Mock<VCConfiguration>();
            configuration.Setup(c => c.ConfigurationName).Returns(configurationName);
            configuration.Setup(c => c.Platform).Returns(new Platform { Name = platformName });
            configuration.Setup(c => c.DebugSettings).Returns(debugSettings.Object);
            configuration.Setup(c => c.Evaluate(It.IsAny<string>())).Returns((string str) => str);
            configuration.Setup(c => c.Name).Returns("Name");

            return configuration.Object;
        }

        //---------------------------------------------------------------------
        public class Platform
        {
            public string Name { get; set; }
        }

        //---------------------------------------------------------------------
        static SolutionBuild BuildSolutionBuildMock()
        {
            var solutionContext = new Mock<SolutionContext>();
            solutionContext.Setup(s => s.ProjectName).Returns(projectName);
            solutionContext.Setup(s => s.ShouldBuild).Returns(true);
            solutionContext.Setup(s => s.ConfigurationName).Returns(configurationName);
            solutionContext.Setup(s => s.PlatformName).Returns(platformName);

            var solutionContexts = new Mock<SolutionContexts>();
            var solutionContextCollection = new List<SolutionContext> { solutionContext.Object };
            var solutionContextCollectionEnumerable = solutionContexts.As<System.Collections.IEnumerable>();
            solutionContextCollectionEnumerable.Setup(s => s.GetEnumerator()).Returns(solutionContextCollection.GetEnumerator());

            var solutionConfiguration = new Mock<SolutionConfiguration2>();
            solutionConfiguration.Setup(s => s.SolutionContexts).Returns(solutionContexts.Object);
            solutionConfiguration.SetupGet(s => s.PlatformName).Returns("x86");

            var solutionBuild = new Mock<SolutionBuild2>();
            solutionBuild.As<SolutionBuild>().Setup(s => s.StartupProjects).Returns(new object[] { projectName });
            solutionBuild.Setup(s => s.ActiveConfiguration).Returns(solutionConfiguration.Object);
                                   
            return solutionBuild.Object;
        }
    }
}
