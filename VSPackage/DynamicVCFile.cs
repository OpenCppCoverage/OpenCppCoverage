namespace OpenCppCoverage.VSPackage
{
    class DynamicVCFile
    {
        //---------------------------------------------------------------------
        public DynamicVCFile(dynamic file)
        {
            file_ = file;
        }

        //---------------------------------------------------------------------
        public string FullPath
        {
            get
            {
                return file_.FullPath;
            }
        }

        readonly dynamic file_;
    }
}
