using System.Collections.Generic;

namespace OpenCppCoverage.VSPackage
{
    class Settings
    {
        public string WorkingDir { get; set; }
        public string Arguments { get; set; }
        public string Command { get; set; }
        public IEnumerable<string> ModulePaths { get; set; }
        public IEnumerable<string> SourcePaths { get; set; }
        public string ConfigurationName { get; set; }
        public string ProjectName { get; set; }
    }
}
