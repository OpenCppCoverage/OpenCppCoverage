using EnvDTE;
using Microsoft.VisualStudio.VCProjectEngine;
using System.Collections.Generic;
using System.Linq;

namespace OpenCppCoverage.VSPackage
{
    class ExtendedProject
    {        
        //---------------------------------------------------------------------
        public ExtendedProject(Project project, VCProject vcProject)
        {
            this.project = project;
            this.vcProject = vcProject;            
        }

        //---------------------------------------------------------------------
        public string Name 
        { 
            get
            {
                return project.Name;
            }
        }

        //---------------------------------------------------------------------
        public string UniqueName
        {
            get
            {
                return project.UniqueName;
            }
        }

        //---------------------------------------------------------------------
        public string ProjectDirectory
        {
            get
            {
                return vcProject.ProjectDirectory;
            }
        }

        //---------------------------------------------------------------------
        public VCConfiguration FindConfiguration(string configurationName)
        {
            return this.Configurations.FirstOrDefault(
                c => c.ConfigurationName == configurationName);            
        }

        //---------------------------------------------------------------------
        public List<VCConfiguration> Configurations
        {
            get
            {
                var configurations = new List<VCConfiguration>();
                foreach (VCConfiguration configuration in vcProject.Configurations)
                    configurations.Add(configuration);
                
                return configurations;
            }
        }
        
        readonly Project project;
        readonly VCProject vcProject;
    }
}
