using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VSSDK.Tools.VsIdeTesting;
using OpenCppCoverage.VSPackage;
using System.Linq;

namespace VSPackage_IntegrationTests
{
    [TestClass()]
    public class ConfigurationManagerTest
    {
        //---------------------------------------------------------------------
        [TestMethod]
        [HostType("VS IDE")]
        public void ProjectNotMarkedAsBuild()
        {            
            var configuration = TestHelpers.OpenDefaultSolution(
                TestHelpers.CppConsoleApplication, ConfigurationName.Release, PlatFormName.Win32);            
            var error = GetConfigurationError(configuration);
            Assert.AreEqual(string.Format(
                "The project {0} is marked" + 
                " as not build for the active solution configuration. Please check your " + 
                "solution Configuration Manager.", TestHelpers.CppConsoleApplication), error);                    
        }

        //---------------------------------------------------------------------
        static string GetConfigurationError(EnvDTE.SolutionConfiguration solutionConfiguration)
        {
            var solution = VsIdeTestHostContext.Dte.Solution;
            var configurationManager = new ConfigurationManager(solutionConfiguration);            
            var project = solution.Projects.Cast<EnvDTE.Project>().First(p => p.UniqueName == TestHelpers.CppConsoleApplication);
            var extendedProject = new ExtendedProject(project, project.Object as Microsoft.VisualStudio.VCProjectEngine.VCProject);

            try
            {
                configurationManager.GetConfiguration(extendedProject);
                return null;
            }
            catch (VSPackageException e)
            {
                return e.Message;
            }
        }
    }
}
