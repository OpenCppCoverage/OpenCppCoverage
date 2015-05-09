using EnvDTE80;
using Microsoft.VisualStudio.VCProjectEngine;
using Microsoft.VSSDK.Tools.VsIdeTesting;
using System;
using System.Collections;
using System.Linq;

namespace VSPackage_IntegrationTests
{
    class SolutionConfiguration
    {
        //---------------------------------------------------------------------
        public enum Configuration
        {
            Debug,
            Release
        }

        //---------------------------------------------------------------------
        public enum PlatForm
        {
            Win32,
            x64
        }

        //---------------------------------------------------------------------
        static public void ExecuteUnderConfiguration(
            Configuration configuration,
            PlatForm platform,
            Action action)
        {
            var solutionActiveConfiguration = VsIdeTestHostContext.Dte.Solution.SolutionBuild.ActiveConfiguration;

            try
            {
                SolutionConfiguration.SetActiveSolutionConfiguration(configuration, platform);
                action();
            }
            finally
            {
                solutionActiveConfiguration.Activate();
            }
        }

        //---------------------------------------------------------------------
        static public VCDebugSettings GetCurrentDebugSettings(string applicationName)
        {
            var configuration = GetCurrentConfiguration(applicationName);
            return (VCDebugSettings)configuration.DebugSettings;
        }

        //---------------------------------------------------------------------
        static public void CleanSolution()
        {
            VsIdeTestHostContext.Dte.Solution.SolutionBuild.Clean(true);
        }

        //---------------------------------------------------------------------
        static VCConfiguration GetCurrentConfiguration(string applicationName)
        {
            var projects = VsIdeTestHostContext.Dte.Solution.Projects.Cast<EnvDTE.Project>();
            var cppConsoleApplication = projects.First(p => p.UniqueName == applicationName);
            var vcCppConsoleApplication = (VCProject)cppConsoleApplication.Object;
            var configurations = (IEnumerable)vcCppConsoleApplication.Configurations;
            var solutionConfiguration =
                VsIdeTestHostContext.Dte.Solution.SolutionBuild.ActiveConfiguration as SolutionConfiguration2;
            
            return configurations.Cast<VCConfiguration>().First(
                c => c.ConfigurationName == solutionConfiguration.Name && c.Platform.Name == solutionConfiguration.PlatformName);            
        }

        //---------------------------------------------------------------------
        static void SetActiveSolutionConfiguration(
            Configuration configuration,
            PlatForm platform)
        {            
            var dte = VsIdeTestHostContext.Dte;

            var configurations = dte.Solution.SolutionBuild.SolutionConfigurations.Cast<SolutionConfiguration2>();
            var configurationToActivate = configurations.First(
                c => c.Name == configuration.ToString() && c.PlatformName == platform.ToString());
            configurationToActivate.Activate();
        }

    }
}
