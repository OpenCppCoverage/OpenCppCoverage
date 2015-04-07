using EnvDTE;
using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.Collections.Generic;
using System.IO;
using VSPackageUnManagedWrapper;

namespace OpenCppCoverage.VSPackage
{
    class OpenCppCoverageRunner
    {
        //---------------------------------------------------------------------
        public OpenCppCoverageRunner(
            EnvDTE.DTE dte, 
            IVsWebBrowsingService webBrowsingService,
            SettingsBuilder settingsBuilder,
            ErrorHandler errorHandler,
            OutputWindowWriter outputWindowWriter)
        {
            dte_ = dte;
            webBrowsingService_ = webBrowsingService;
            settingsBuilder_ = settingsBuilder;
            errorHandler_ = errorHandler;
            outputWindowWriter_ = outputWindowWriter;
        }

        //---------------------------------------------------------------------
        public void RunCoverageOnStartupProject()
        {            
            var buildContext = new BuildContext();
            _dispBuildEvents_OnBuildProjConfigDoneEventHandler onBuildDone = 
                (string project, string projectConfig, string platform, string solutionConfig, bool success)
                    => OnBuildProjConfigDone(project, projectConfig, platform, solutionConfig, success, buildContext);

            buildContext.OnBuildDone = onBuildDone;
            var settings = settingsBuilder_.ComputeSettings();
            buildContext.Settings = settings;
            
            dte_.Events.BuildEvents.OnBuildProjConfigDone += onBuildDone;

            LogSettings(settings);
            outputWindowWriter_.WriteLine("Start building " + settings.ProjectName);

            var solutionBuild = dte_.Solution.SolutionBuild;
            solutionBuild.BuildProject(settings.ConfigurationName, settings.ProjectName, false);
        }

        //---------------------------------------------------------------------
        void OnBuildProjConfigDone(
            string project, 
            string projectConfig, 
            string platform, 
            string solutionConfig, 
            bool success, 
            BuildContext buildContext)
        {
            // This method is executed asynchronously and so we need to catch errors.
            errorHandler_.Execute(() =>
                {                    
                    dte_.Events.BuildEvents.OnBuildProjConfigDone -= buildContext.OnBuildDone;
                    outputWindowWriter_.ActivatePane();

                    if (!success)
                        throw new VSPackageException("Build failed.");
                    
                    outputWindowWriter_.WriteLine("Start code coverage...");
                    var codeCoverageRunner = new VSPackageUnManagedWrapper.CodeCoverageRunner();
                    var settings = buildContext.Settings;

                    CheckSettings(settings);
                    var outputPath = codeCoverageRunner.Run(
                        settings.Command,
                        settings.Arguments,
                        settings.WorkingDir,
                        settings.ModulePaths,
                        settings.SourcePaths,
			            outputWindowWriter_);

                    outputWindowWriter_.WriteLine("Report was generating at " + outputPath);
                    ShowCoverage(outputPath);
                });
        }

        //---------------------------------------------------------------------        
        void CheckSettings(Settings settings)
        {
            if (!File.Exists(settings.Command))
            {
                throw new VSPackageException(
                    string.Format(@"Debug command ""{0}"" does not exit.", settings.Command));
            }

            if (!string.IsNullOrEmpty(settings.WorkingDir) && !Directory.Exists(settings.WorkingDir))
            {
                throw new VSPackageException(
                    string.Format(@"Debug working directory ""{0}"" does not exit.", settings.WorkingDir));
            }
        }

        //---------------------------------------------------------------------
        class BuildContext
        {            
            public _dispBuildEvents_OnBuildProjConfigDoneEventHandler OnBuildDone { get; set; }
            public Settings Settings { get; set; }
        }

        //---------------------------------------------------------------------
        void ShowCoverage(string url)
        {            
            IVsWindowFrame pFrame = null;

            if (Microsoft.VisualStudio.ErrorHandler.Failed(
                webBrowsingService_.Navigate(url, (uint)__VSWBNAVIGATEFLAGS.VSNWB_ForceNew, out pFrame)))
            {
                throw new Exception("Cannot open code coverage results: " + url);
            }
        }

        //---------------------------------------------------------------------
        void LogSettings(Settings settings)
        {                     
            outputWindowWriter_.WriteLine("Current Configuration: ");
            outputWindowWriter_.WriteLine(" - Project Name: " + settings.ProjectName);
            outputWindowWriter_.WriteLine(" - Command: " + settings.Command);
            outputWindowWriter_.WriteLine(" - Arguments: " + settings.Arguments);
            outputWindowWriter_.WriteLine(" - WorkingDir: " + settings.WorkingDir);
            LogCollection(" - Selected folders: ", settings.SourcePaths);
            LogCollection(" - Selected modules: ", settings.ModulePaths);
        }

        //---------------------------------------------------------------------
        void LogCollection(string name, IEnumerable<string> values)
        {
            outputWindowWriter_.WriteLine(name);
            foreach (var v in values)
                outputWindowWriter_.WriteLine("\t" + v); 
        }

        readonly SettingsBuilder settingsBuilder_;
        readonly EnvDTE.DTE dte_;
        readonly OutputWindowWriter outputWindowWriter_;
        readonly ErrorHandler errorHandler_;
        readonly IVsWebBrowsingService webBrowsingService_;
    }
}
