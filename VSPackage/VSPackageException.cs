using System;

namespace OpenCppCoverage.VSPackage
{
    [Serializable]
    class VSPackageException: Exception
    {
        public VSPackageException(string message)
            : base(message)
        {
        }
    }
}
