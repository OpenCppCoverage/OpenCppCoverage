Invoke-WebRequest -OutFile nuget.exe https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
Invoke-WebRequest -OutFile ThirdParty.1.4.0.nupkg https://github.com/OpenCppCoverage/OpenCppCoverageThirdParty/releases/download/1.4.0/ThirdParty.1.4.0.nupkg

$scriptFolder = Split-Path $script:MyInvocation.MyCommand.Path
Invoke-Expression "./nuget.exe install ThirdParty -Source '$scriptFolder' -OutputDirectory packages"