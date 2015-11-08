using System.Collections.Generic;

namespace OpenCppCoverage.VSPackage
{
    class DynamicVCProject
    {
        //---------------------------------------------------------------------
        public DynamicVCProject(dynamic project) 
        {
            project_ = project;
        }

        //---------------------------------------------------------------------
        public List<DynamicVCConfiguration> Configurations
        {
            get
            {
                var configurations = new List<DynamicVCConfiguration>();
                foreach (var configuration in project_.Configurations)
                    configurations.Add(new DynamicVCConfiguration(configuration));

                return configurations;
            }
        }

        //---------------------------------------------------------------------
        public List<DynamicVCFile> Files
        {
            get
            {
                var files = new List<DynamicVCFile>();
                foreach (var file in project_.Files)
                    files.Add(new DynamicVCFile(file));
                return files;
            }
        }

        readonly dynamic project_;
    }
}
