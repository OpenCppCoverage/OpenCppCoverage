namespace OpenCppCoverage.VSPackage
{
    class DynamicVCConfiguration
    {
        //---------------------------------------------------------------------
        public DynamicVCConfiguration(dynamic configuration)
        {
            configuration_ = configuration;
            debugSettings_ = new DynamicVCDebugSettings(configuration_.DebugSettings);
        }

        //---------------------------------------------------------------------
        public string ConfigurationName
        {
            get
            {
                return configuration_.ConfigurationName;
            }
        }

        //---------------------------------------------------------------------
        public string PlatformName
        {
            get
            {
                return configuration_.Platform.Name;
            }
        }

        //---------------------------------------------------------------------
        public string Evaluate(string str)
        {
            return configuration_.Evaluate(str);
        }

        //---------------------------------------------------------------------
        public DynamicVCDebugSettings DebugSettings
        {
            get
            {
                return debugSettings_;
            }
        }

        //---------------------------------------------------------------------
        public string PrimaryOutput
        {
            get
            {
                return configuration_.PrimaryOutput;
            }
        }

        readonly dynamic configuration_;
        readonly DynamicVCDebugSettings debugSettings_;
    }
}
