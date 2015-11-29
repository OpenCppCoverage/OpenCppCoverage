using EnvDTE80;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.Win32;
using System;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;

namespace OpenCppCoverage.VSPackage
{
    /// <summary>
    /// This is the class that implements the package exposed by this assembly.
    ///
    /// The minimum requirement for a class to be considered a valid package for Visual Studio
    /// is to implement the IVsPackage interface and register itself with the shell.
    /// This package uses the helper classes defined inside the Managed Package Framework (MPF)
    /// to do it: it derives from the Package class that provides the implementation of the 
    /// IVsPackage interface and uses the registration attributes defined in the framework to 
    /// register itself and its components with the shell.
    /// </summary>
    // This attribute tells the PkgDef creation utility (CreatePkgDef.exe) that this class is
    // a package.
    [PackageRegistration(UseManagedResourcesOnly = true)]
    // This attribute is used to register the information needed to show this package
    // in the Help/About dialog of Visual Studio.
    [InstalledProductRegistration("#110", "#112", "1.0", IconResourceID = 400)]
    // This attribute is needed to let the shell know that this package exposes some menus.
    [ProvideMenuResource("Menus.ctmenu", 1)]
    [Guid(GuidList.guidVSPackagePkgString)]
    public sealed class VSPackagePackage : Package
    {
        /// <summary>
        /// Default constructor of the package.
        /// Inside this method you can place any initialization code that does not require 
        /// any Visual Studio service because at this point the package object is created but 
        /// not sited yet inside Visual Studio environment. The place to do all the other 
        /// initialization is the Initialize method.
        /// </summary>
        public VSPackagePackage()
        {
            Debug.WriteLine(string.Format(CultureInfo.CurrentCulture, "Entering constructor for: {0}", this.ToString()));
        }



        /////////////////////////////////////////////////////////////////////////////
        // Overridden Package Implementation
        #region Package Members

        /// <summary>
        /// Initialization of the package; this method is called right after the package is sited, so this is the place
        /// where you can put all the initialization code that rely on services provided by VisualStudio.
        /// </summary>
        protected override void Initialize()
        {
            Debug.WriteLine (string.Format(CultureInfo.CurrentCulture, "Entering Initialize() of: {0}", this.ToString()));
            base.Initialize();

            // Add our command handlers for menu (commands must exist in the .vsct file)
            OleMenuCommandService mcs = GetService(typeof(IMenuCommandService)) as OleMenuCommandService;
            if ( null != mcs )
            {
                // Create the command for the menu item.
                CommandID menuCommandID = new CommandID(GuidList.guidVSPackageCmdSet, (int)PkgCmdIDList.RunOpenCppCoverageCommand);
                MenuCommand menuItem = new MenuCommand(MenuItemCallback, menuCommandID );
                mcs.AddCommand( menuItem );
            }
        }
        #endregion

        /// <summary>
        /// This function is the callback used to execute a command when the a menu item is clicked.
        /// See the Initialize method to see how the menu item is associated to this function using
        /// the OleMenuCommandService service and the MenuCommand class.
        /// </summary>
        private void MenuItemCallback(object sender, EventArgs e)
        {
            IVsUIShell uiShell = (IVsUIShell)GetService(typeof(SVsUIShell));
            
            var errorHandler = new ErrorHandler(uiShell);                      
            errorHandler.Execute(() =>
            {
                var dte = (DTE2)GetService(typeof(EnvDTE.DTE));
                var outputWindow = (IVsOutputWindow)GetService(typeof(SVsOutputWindow));
                var outputWriter = new OutputWindowWriter(dte, outputWindow);

                errorHandler.OutputWriter = outputWriter;
                var webBrowsingService = (IVsWebBrowsingService)GetService(typeof(IVsWebBrowsingService));
                var settingsBuilder = new SettingsBuilder((Solution2)dte.Solution);

                var openCppCoverageRunner = new CoverageRunner(dte, webBrowsingService, settingsBuilder, errorHandler, outputWriter);

                CheckVCRedistInstalled();
                openCppCoverageRunner.RunCoverageOnStartupProject();
            });                                             
        }

        //---------------------------------------------------------------------
        static void CheckVCRedistInstalled()
        {
            if (!IsVCRedistInstalled("x86")
              || (Environment.Is64BitOperatingSystem && !IsVCRedistInstalled("x64")))
            {
                throw new VSPackageException("Cannot start OpenCppCoverage. " +
                        "You need to install Visual Studio 2015 redistributable " +
                        "vc_redist.x86.exe and vc_redist.x64.exe (for 64 bits operating system) " +
                        "and restart Visual Studio: " +
                        "https://www.microsoft.com/en-US/download/details.aspx?id=48145");
            }
        }

        //---------------------------------------------------------------------
        static bool IsVCRedistInstalled(string architecture)
        {
            const string runtimeKey = @"SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\";

            using (var subKey = Registry.LocalMachine.OpenSubKey(runtimeKey + architecture))
            {
                if (subKey != null)
                {
                    var installedValueObject = subKey.GetValue("Installed");
                    int installedValue = Convert.ToInt32(installedValueObject);
                    return installedValue == 1;
                }
            }

            return false;
        }
    }
}
