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

using EnvDTE;
using EnvDTE80;
using Microsoft.VisualStudio.Shell.Interop;
using System;

namespace OpenCppCoverage.VSPackage
{
    class OutputWindowWriter
    {
        //---------------------------------------------------------------------
        public OutputWindowWriter(DTE2 dte, IVsOutputWindow outputWindow)
        {
            // These lines show the output windows
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
        public readonly static Guid OpenCppCoverageOutputPaneGuid = new Guid("CB47C727-5E45-467B-A4CD-4A025986A8A0");
    }
}
