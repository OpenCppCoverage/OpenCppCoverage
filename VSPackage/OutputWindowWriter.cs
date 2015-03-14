using EnvDTE;
using Microsoft.VisualStudio.Shell.Interop;
using System;

namespace OpenCppCoverage.VSPackage
{
    class OutputWindowWriter : VSPackageUnManagedWrapper.ILogger
    {
        //---------------------------------------------------------------------
        public OutputWindowWriter(EnvDTE.DTE dte, IVsOutputWindow outputWindow)
        {
            // These line show the output windows
            Window output = dte.Windows.Item(EnvDTE.Constants.vsWindowKindOutput);
            output.Activate();

            if (Microsoft.VisualStudio.ErrorHandler.Failed(outputWindow.GetPane(OpenCppCoverageOutputPaneGuid, out outputWindowPane_)))
            {
                if (Microsoft.VisualStudio.ErrorHandler.Failed(outputWindow.CreatePane(OpenCppCoverageOutputPaneGuid, "OpenCppCoverage", 1, 1)))
                    throw new Exception("Cannot create new pane.");

                if (Microsoft.VisualStudio.ErrorHandler.Failed(outputWindow.GetPane(OpenCppCoverageOutputPaneGuid, out outputWindowPane_)))
                    throw new Exception("Cannot get the pane.");
            }

            outputWindowPane_.Clear();
            ActivatePane();
        }

        //---------------------------------------------------------------------
        public void ActivatePane()
        {
            outputWindowPane_.Activate();
        }

        //---------------------------------------------------------------------
        public bool WriteLine(string message)
        {
            return Microsoft.VisualStudio.ErrorHandler.Succeeded(outputWindowPane_.OutputString(message + "\n"));
        }
        
        readonly IVsOutputWindowPane outputWindowPane_;
        readonly static Guid OpenCppCoverageOutputPaneGuid = new Guid("CB47C727-5E45-467B-A4CD-4A025986A8A0");
    }
}
