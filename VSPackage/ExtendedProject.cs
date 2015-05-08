using EnvDTE;
using Microsoft.VisualStudio.VCProjectEngine;
using System.Collections.Generic;

namespace OpenCppCoverage.VSPackage
{
    class ExtendedProject
    {        
        //---------------------------------------------------------------------
        public ExtendedProject(Project project, VCProject vcProject)
        {
            this.project_ = project;
            this.vcProject_ = vcProject;
        }

        //---------------------------------------------------------------------
        public string Name 
        { 
            get
            {
                return project_.Name;
            }
        }

        //---------------------------------------------------------------------
        public string UniqueName
        {
            get
            {
                return project_.UniqueName;
            }
        }

        //---------------------------------------------------------------------
        public string ProjectDirectory
        {
            get
            {
                return vcProject_.ProjectDirectory;
            }
        }
        
        //---------------------------------------------------------------------
        public List<VCConfiguration> Configurations
        {
            get
            {
                var configurations = new List<VCConfiguration>();
                foreach (VCConfiguration configuration in vcProject_.Configurations)
                    configurations.Add(configuration);

                return configurations;
            }
        }
        
        readonly Project project_;
        readonly VCProject vcProject_;        
    }
}
