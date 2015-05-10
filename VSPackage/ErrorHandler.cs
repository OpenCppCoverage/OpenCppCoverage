// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
