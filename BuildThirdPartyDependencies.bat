@echo off
setlocal

SET ROOT_FOLDER=%~dp0/Build/ThirdParty/

cd Build/ThirdParty

IF EXIST vcpkg GOTO REPO_EXISTS
git clone https://github.com/Microsoft/vcpkg.git
:REPO_EXISTS

cd vcpkg
git fetch
git checkout 45d9d68a5825c78d71d8508ff9554d2552049138 .

IF EXIST vcpkg.exe GOTO VCPKG_EXISTS
	.\bootstrap-vcpkg.bat
:VCPKG_EXISTS

.\vcpkg install poco:x64-windows poco:x86-windows
.\vcpkg install protobuf:x64-windows protobuf:x86-windows

.\vcpkg export ^
	poco:x64-windows poco:x86-windows ^
	protobuf:x64-windows protobuf:x86-windows ^
	--nuget --nuget-id=ThirdParty --nuget-version=1.0.0

downloads\nuget-4.1.0\nuget.exe install ThirdParty -Source %ROOT_FOLDER%\vcpkg -OutputDirectory ..\..\..\packages