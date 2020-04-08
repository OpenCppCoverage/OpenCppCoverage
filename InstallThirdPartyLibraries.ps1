Invoke-WebRequest -OutFile nuget.exe https://dist.nuget.org/win-x86-commandline/latest/nuget.exe

# Use Dropbox to store NuGet package as the size of the package exceeds NuGet and Chocolatey maximum size.
Invoke-WebRequest -OutFile ThirdParty.1.4.0.nupkg https://uc62205b5580afc399c3b46e2f5d.dl.dropboxusercontent.com/cd/0/get/A02PiH3khoroQY3tjeUAgpsRKTLSeTpsBKpsfnap2JFHou8YFDryVYbugj_4THviqmzURLK_F18WKAyaMLDkly-Du6azKONZdBKPcJ6AuNyvK-apjxtQs5t3GDGVllpYTJo/file?dl=1

$scriptFolder = Split-Path $script:MyInvocation.MyCommand.Path
Invoke-Expression "./nuget.exe install ThirdParty -Source $scriptFolder -OutputDirectory packages"