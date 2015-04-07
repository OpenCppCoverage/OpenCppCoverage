using EnvDTE;
using Microsoft.VisualStudio.VCProjectEngine;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using VSPackageUnManagedWrapper;

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
            var solutionActiveConfigurationName = GetActiveSolutionName();
            var startupConfiguration = GetStartupConfiguration(solutionActiveConfigurationName, startupProject);
            var debugSettings = startupConfiguration.DebugSettings as VCDebugSettings;

            if (debugSettings == null)
                throw new Exception("DebugSettings is null");
            var settings = new Settings
            {            
                WorkingDir = startupConfiguration.Evaluate(debugSettings.WorkingDirectory),
                Arguments = startupConfiguration.Evaluate(debugSettings.CommandArguments),
                Command = startupConfiguration.Evaluate(debugSettings.Command),
                ConfigurationName = startupConfiguration.ConfigurationName,
                ProjectName = startupProject.UniqueName,                
            };

            SetFilters(projects, solutionActiveConfigurationName, settings);
            
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
        VCConfiguration GetStartupConfiguration(string solutionActiveConfigurationName, ExtendedProject project)
        {
            var configuration = project.FindConfiguration(solutionActiveConfigurationName);

            if (configuration == null)
            {
                var error = new StringBuilder();
                error.AppendLine("Cannot find configuration for your project.");
                error.AppendLine(" - Solution configuration: " + solutionActiveConfigurationName);

                var configurations = project.Configurations;
                var configurationNames = configurations.Select(p => p.ConfigurationName);
                error.AppendLine(" - " + project.Name + " configuration: " + string.Join(",", configurationNames));
                error.AppendLine("Please check configuration manager.");

                throw new VSPackageException(error.ToString());
            }

            return configuration;
        }

        //---------------------------------------------------------------------
        string GetActiveSolutionName()
        {            
            var solutionActiveConfiguration = solution_.SolutionBuild.ActiveConfiguration;

            if (solutionActiveConfiguration == null)
                throw new Exception("Cannot get active configuration for the solution.");

            return solutionActiveConfiguration.Name;
        }

        //---------------------------------------------------------------------
        void SetFilters(
            List<ExtendedProject> projects, 
            string solutionActiveConfigurationName, 
            Settings settings)
        {
            var sourcePaths = new List<string>();
            var modulePaths = new List<string>();

            foreach (var project in projects)
            {
                sourcePaths.Add(project.ProjectDirectory);
                var configuration = project.FindConfiguration(solutionActiveConfigurationName);

                if (configuration != null)
                    modulePaths.Add(configuration.PrimaryOutput);
            }

            settings.SourcePaths = sourcePaths;
            settings.ModulePaths = modulePaths;
        }
               
        Solution solution_;
    }
}
