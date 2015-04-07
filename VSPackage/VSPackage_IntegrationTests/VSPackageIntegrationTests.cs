using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VisualStudio.VCProjectEngine;
using Microsoft.VSSDK.Tools.VsIdeTesting;
using System;
using System.Text;

namespace VSPackage_IntegrationTests
{
    [TestClass()]
    public class VSPackageIntegrationTests
    {
        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void EmptySolution()
        {
            var solutionService = TestHelpers.GetService<IVsSolution>();
            solutionService.CloseSolutionElement((uint)__VSSLNSAVEOPTIONS.SLNSAVEOPT_NoSave, null, 0);
            Assert.AreEqual("OpenCppCoverage\n\nCannot get startup projects", TestHelpers.GetOpenCppCoverageMessage());
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void NotCppStartupProject()
        {
            TestHelpers.OpenDefaultSolution(TestHelpers.CSharpConsoleApplication);
            Assert.AreEqual("OpenCppCoverage\n\nNo C++ startup project found.", TestHelpers.GetOpenCppCoverageMessage());
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void SeveralStartupProjects()
        {
            TestHelpers.OpenDefaultSolution(TestHelpers.CppConsoleApplication, TestHelpers.CppConsoleApplication2);         
            var message = new StringBuilder("OpenCppCoverage\n\nYou cannot run OpenCppCoverage for several projects:\n");
            message.Append(" - " + TestHelpers.CppConsoleApplication + '\n');
            message.Append(" - " + TestHelpers.CppConsoleApplication2);

            Assert.AreEqual(message.ToString(), TestHelpers.GetOpenCppCoverageMessage());
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void WrongConfiguration()
        {
            TestHelpers.OpenDefaultSolution(TestHelpers.CppConsoleApplication);
            var solutionActiveConfiguration = VsIdeTestHostContext.Dte.Solution.SolutionBuild.ActiveConfiguration;

            try
            {
                TestHelpers.SetActiveSolutionConfiguration("Release");
                Assert.AreEqual("OpenCppCoverage\n\nCannot find configuration for your project.\r\n"
                    + " - Solution configuration: Release\r\n - CppConsoleApplication configuration: Debug\r\n"
                    + "Please check configuration manager.", TestHelpers.GetOpenCppCoverageMessage());
            }
            finally
            {
                solutionActiveConfiguration.Activate();
            }
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void DoesNotCompile()
        {            
            TestHelpers.OpenDefaultSolution(TestHelpers.CppConsoleApplication2);
            Assert.AreEqual("OpenCppCoverage\n\nBuild failed.", TestHelpers.GetOpenCppCoverageMessage());
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void InvalidWorkingDirectory()
        {
            CheckInvalidSettings(
                (debugSettings, v) => debugSettings.WorkingDirectory = v, 
                debugSettings => debugSettings.WorkingDirectory, 
                "OpenCppCoverage\n\nDebug working directory \"{0}\" does not exit.");
        }

        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void InvalidCommand()
        {
            CheckInvalidSettings(
                (debugSettings, v) => debugSettings.Command = v,
                debugSettings => debugSettings.Command,
                "OpenCppCoverage\n\nDebug command \"{0}\" does not exit.");
        }

        //---------------------------------------------------------------------
        static void CheckInvalidSettings(
            Action<VCDebugSettings, string> setter, 
            Func<VCDebugSettings, string> getter, 
            string expectedMessage)
        {
            TestHelpers.OpenDefaultSolution(TestHelpers.CppConsoleApplication);
            var debugSettings = TestHelpers.GetCppConsoleApplicationDebugSettings();
            const string InvalidValue = "InvalidValue";
            var oldValue = getter(debugSettings);
            try
            {
                setter(debugSettings, InvalidValue);

                var fullExpectedMessage = string.Format(expectedMessage, InvalidValue);
                Assert.AreEqual(fullExpectedMessage, TestHelpers.GetOpenCppCoverageMessage());
            }
            finally
            {
                setter(debugSettings, oldValue);
            }
        }
    }
}
