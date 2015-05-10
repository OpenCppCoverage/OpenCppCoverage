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
using Microsoft.VisualStudio.VCProjectEngine;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace OpenCppCoverage.VSPackage
{
    class SettingsBuilder
    {
        //---------------------------------------------------------------------
        public SettingsBuilder(EnvDTE.DTE dte)
        {
            solution_ = dte.Solution;
        }

        //---------------------------------------------------------------------
        public Settings ComputeSettings()
        {
            var projects = GetProjects();
            var startupProject = GetStartupProject(projects);
            var configurationManager = new ConfigurationManager(solution_.SolutionBuild.ActiveConfiguration);
            var startupConfiguration = configurationManager.GetConfiguration(startupProject);
            var debugSettings = startupConfiguration.DebugSettings as VCDebugSettings;

            if (debugSettings == null)
                throw new Exception("DebugSettings is null");
            var settings = new Settings
            {            
                WorkingDir = startupConfiguration.Evaluate(debugSettings.WorkingDirectory),
                Arguments = startupConfiguration.Evaluate(debugSettings.CommandArguments),
                Command = startupConfiguration.Evaluate(debugSettings.Command),
                ConfigurationName = startupConfiguration.Name,
                ProjectName = startupProject.UniqueName,                
            };

            SetFilters(configurationManager, projects, settings);
            
            return settings;                                                 
        }

        //---------------------------------------------------------------------
        List<ExtendedProject> GetProjects()
        {
            var projects = new List<ExtendedProject>();
            
            foreach (Project project in solution_.Projects)
            {
                var vcProject = project.Object as VCProject;

                if (vcProject != null)
                    projects.Add(new ExtendedProject(project, vcProject));
            }

            return projects;
        }

        //---------------------------------------------------------------------
        ExtendedProject GetStartupProject(List<ExtendedProject> projects)
        {
            var startupProjects = GetStartupProjects(projects);

            if (startupProjects.Count == 0)
                throw new VSPackageException("No C++ startup project found.");
            
            if (startupProjects.Count != 1)
            {
                var error = new StringBuilder();

                error.Append("You cannot run OpenCppCoverage for several projects:\n");
                foreach (var project in startupProjects)
                    error.Append(" - " + project.UniqueName + "\n");
                throw new VSPackageException(error.ToString());
            }

            return startupProjects[0];
        }

        //---------------------------------------------------------------------
        List<ExtendedProject> GetStartupProjects(List<ExtendedProject> projects)
        {            
            var startupProjectsNames = solution_.SolutionBuild.StartupProjects as object[];

            if (startupProjectsNames == null)
                throw new VSPackageException("Cannot get startup projects");

            var startupProjectsSet = new HashSet<String>();
            foreach (String projectName in startupProjectsNames)
                startupProjectsSet.Add(projectName);

            return projects.Where(p => startupProjectsSet.Contains(p.UniqueName)).ToList();
        }

        //---------------------------------------------------------------------
        static void SetFilters(
            ConfigurationManager configurationManager,
            List<ExtendedProject> projects,             
            Settings settings)
        {
            var sourcePaths = new List<string>();
            var modulePaths = new List<string>();

            foreach (var project in projects)
            {
                sourcePaths.Add(project.ProjectDirectory);
                var configuration = configurationManager.FindConfiguration(project);

                if (configuration != null)
                    modulePaths.Add(configuration.PrimaryOutput);
            }

            settings.SourcePaths = sourcePaths;
            settings.ModulePaths = modulePaths;
        }
               
        Solution solution_;
    }
}
