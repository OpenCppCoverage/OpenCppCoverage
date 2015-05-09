using EnvDTE80;
using Microsoft.VisualStudio.VCProjectEngine;
using Microsoft.VSSDK.Tools.VsIdeTesting;
using System;
using System.Collections;
using System.Linq;

namespace VSPackage_IntegrationTests
{
    //---------------------------------------------------------------------
    public enum ConfigurationName
    {
        Debug,
        Release
    }

    //---------------------------------------------------------------------
    public enum PlatFormName
    {
        Win32,
        x64
    }

    //---------------------------------------------------------------------
    class SolutionConfigurationHelpers
    {                
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
        static public SolutionConfiguration2 GetSolutionConfiguration(
            ConfigurationName configurationName,
            PlatFormName platformName)
        {
            var dte = VsIdeTestHostContext.Dte;
            var configurations = dte.Solution.SolutionBuild.SolutionConfigurations.Cast<SolutionConfiguration2>();
    
            return configurations.First(
                c => c.Name == configurationName.ToString() && c.PlatformName == platformName.ToString());
        }

        //---------------------------------------------------------------------
        static public SolutionConfiguration2 SetActiveSolutionConfiguration(
            ConfigurationName configurationName,
            PlatFormName platformName)
        {
            var configurationToActivate = GetSolutionConfiguration(configurationName, platformName);
            configurationToActivate.Activate();

            return configurationToActivate;
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
    }
}
