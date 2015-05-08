using Microsoft.VisualStudio.Shell.Interop;
using System;

namespace OpenCppCoverage.VSPackage
{
    class ErrorHandler
    {
        //---------------------------------------------------------------------
        public ErrorHandler(IVsUIShell uiShell)
        {
            uiShell_ = uiShell;
        }

        //---------------------------------------------------------------------
        public OutputWindowWriter OutputWriter { get; set; }

        //---------------------------------------------------------------------
        public void Execute(Action action)
        {
            try
            {
                action();
            }
            catch (VSPackageException exception)
            {
                if (OutputWriter != null)
                    OutputWriter.WriteLine(exception.Message);
                ShowMessage(exception.Message);
            }
            catch (Exception exception)
            {
                if (OutputWriter != null && OutputWriter.WriteLine(exception.ToString()))             
                    ShowMessage("Unknow error. Please see the output console for more information.");
                else
                    ShowMessage(exception.ToString());
            }                        
        }

        //---------------------------------------------------------------------
        void ShowMessage(string message)
        {
            Guid clsid = Guid.Empty;
            int result;
            Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(uiShell_.ShowMessageBox(
                       0,
                       ref clsid,
                       "OpenCppCoverage",
                       message,
                       string.Empty,
                       0,
                       OLEMSGBUTTON.OLEMSGBUTTON_OK,
                       OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST,
                       OLEMSGICON.OLEMSGICON_INFO,
                       0, // false
                       out result));
        }

        readonly IVsUIShell uiShell_;
    }
}
