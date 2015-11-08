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
using System;
using System.Linq;
using System.Text;

namespace OpenCppCoverage.VSPackage
{
    class ConfigurationManager
    {
        //---------------------------------------------------------------------
        public ConfigurationManager(SolutionConfiguration2 activeConfiguration)
        {
            if (activeConfiguration == null)
                throw new Exception("SolutionConfiguration is null");
             activeConfiguration_ = activeConfiguration;
        }

        //---------------------------------------------------------------------
        public DynamicVCConfiguration GetConfiguration(ExtendedProject project)
        {
            string error;
            var configuration = ComputeConfiguration(project, out error);
            
            if (configuration == null)
                throw new VSPackageException(error);

            return configuration;
        }

        //---------------------------------------------------------------------
        public DynamicVCConfiguration FindConfiguration(ExtendedProject project)
        {
            string error;
            var configuration = ComputeConfiguration(project, out error);
            return configuration;
        }

        //---------------------------------------------------------------------
        public string GetSolutionConfigurationName()
        {
            return this.activeConfiguration_.Name + '|' + this.activeConfiguration_.PlatformName;

        }

        //---------------------------------------------------------------------
        DynamicVCConfiguration ComputeConfiguration(ExtendedProject project, out string error)
        {
            error = null;
            var context = ComputeContext(project, ref error);

            if (context == null)
                return null;

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
        static DynamicVCConfiguration ComputeConfiguration(
            ExtendedProject project, 
            SolutionContext context, 
            ref string error)
        {
            var configurations = project.Configurations;
            var configuration = configurations.FirstOrDefault(
                c => c.ConfigurationName == context.ConfigurationName && c.PlatformName == context.PlatformName);

            if (configuration == null)
            {
                var builder = new StringBuilder();

                builder.AppendLine(string.Format("Cannot find a configuration for the project {0}", project.UniqueName));
                builder.AppendLine(string.Format(" - Solution: configuration: {0} platform: {1}", context.ConfigurationName, context.PlatformName));
                foreach (var config in configurations)
                    builder.AppendLine(string.Format(" - Project: configuration: {0} platform: {1}", config.ConfigurationName, config.PlatformName));
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
                        + "Please check your solution Configuration Manager.",
                        project.UniqueName);
                return null;
            }

            return context;
        }

        readonly SolutionConfiguration2 activeConfiguration_;
    }
}
