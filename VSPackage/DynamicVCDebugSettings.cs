namespace OpenCppCoverage.VSPackage
{
    class DynamicVCDebugSettings
    {
        //---------------------------------------------------------------------
        public DynamicVCDebugSettings(dynamic settings)
        {
            settings_ = settings;
        }

        //---------------------------------------------------------------------
        public string WorkingDirectory
        {
            get
            {
                return settings_.WorkingDirectory;
            }
        }

        //---------------------------------------------------------------------
        public string CommandArguments
        {
            get
            {
                return settings_.CommandArguments;
            }
        }

        //---------------------------------------------------------------------
        public string Command
        {
            get
            {
                return settings_.Command;
            }
        }


        readonly dynamic settings_;
    }
}
