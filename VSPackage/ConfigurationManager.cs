using EnvDTE;
using Microsoft.VisualStudio.VCProjectEngine;
using System;
using System.Linq;
using System.Text;

namespace OpenCppCoverage.VSPackage
{
    class ConfigurationManager
    {
        //---------------------------------------------------------------------
        public ConfigurationManager(SolutionConfiguration activeConfiguration)
        {
            if (activeConfiguration == null)
                throw new Exception("SolutionConfiguration is null");
             activeConfiguration_ = activeConfiguration;
        }

        //---------------------------------------------------------------------
        public VCConfiguration GetConfiguration(ExtendedProject project)
        {
            string error;
            var configuration = ComputeConfiguration(project, out error);
            
            if (configuration == null)
                throw new VSPackageException(error);

            return configuration;
        }

        //---------------------------------------------------------------------
        public VCConfiguration FindConfiguration(ExtendedProject project)
        {
            string error;
            var configuration = ComputeConfiguration(project, out error);
            return configuration;
        }

        //---------------------------------------------------------------------
        VCConfiguration ComputeConfiguration(ExtendedProject project, out string error)
        {
            error = null;
            var context = ComputeContext(project, ref error);
            
            if (!context.ShouldBuild)
            {
                error = string.Format("The project {0} is marked as not build for the active solution configuration. "
                                + "Please check your solution Configuration Manager.",
                        project.UniqueName);
                return null;
            }

            return ComputeConfiguration(project, context, ref error);
        }

        //---------------------------------------------------------------------
        static VCConfiguration ComputeConfiguration(
            ExtendedProject project, 
            SolutionContext context, 
            ref string error)
        {
            var configurations = project.Configurations;
            var configuration = configurations.FirstOrDefault(
                c => c.ConfigurationName == context.ConfigurationName && c.Platform.Name == context.PlatformName);

            if (configuration == null)
            {
                var builder = new StringBuilder();

                builder.AppendLine(string.Format("Cannot find a configuration for project: {0}", project.UniqueName));
                builder.AppendLine(string.Format("Solution: configuration: {0} platform: {1}", context.ConfigurationName, context.PlatformName));
                foreach (var config in configurations)
                    builder.AppendLine(string.Format("Project: configuration: {0} platform: {1}", config.ConfigurationName, config.Platform.Name));
                error = builder.ToString();
                return null;
            }

            return configuration;
        }

        //---------------------------------------------------------------------
        SolutionContext ComputeContext(ExtendedProject project, ref string error)
        {
            var contexts = activeConfiguration_.SolutionContexts.Cast<SolutionContext>();
            var context = contexts.FirstOrDefault(c => c.ProjectName == project.UniqueName);

            if (context == null)
            {
                error = string.Format("Cannot find {0} in project contexts. "
                        + "Please check your solution Configuration Manager",
                        project.UniqueName);
                return null;
            }

            return context;
        }

        readonly SolutionConfiguration activeConfiguration_;
    }
}
