@echo off
setlocal

SET ROOT_FOLDER=%~dp0/Build/ThirdParty/

IF EXIST "%ROOT_FOLDER%" GOTO THIRD_PARTY_EXISTS
mkdir "%ROOT_FOLDER%"
:THIRD_PARTY_EXISTS

cd Build/ThirdParty

IF EXIST vcpkg GOTO REPO_EXISTS
git clone https://github.com/Microsoft/vcpkg.git
:REPO_EXISTS

cd vcpkg
git fetch
git checkout ed0df8ecc4ed7e755ea03e18aaf285fd9b4b4a74 .

IF EXIST vcpkg.exe GOTO VCPKG_EXISTS
	call .\bootstrap-vcpkg.bat
:VCPKG_EXISTS

.\vcpkg install poco:x64-windows poco:x86-windows
.\vcpkg install protobuf:x64-windows protobuf:x86-windows
.\vcpkg install gtest:x64-windows gtest:x86-windows
.\vcpkg install ctemplate:x64-windows ctemplate:x86-windows
.\vcpkg install boost-optional:x64-windows boost-optional:x86-windows
.\vcpkg install boost-filesystem:x64-windows boost-filesystem:x86-windows
.\vcpkg install boost-algorithm:x64-windows boost-algorithm:x86-windows
.\vcpkg install boost-container:x64-windows boost-container:x86-windows
.\vcpkg install boost-program-options:x64-windows boost-program-options:x86-windows
.\vcpkg install boost-regex:x64-windows boost-regex:x86-windows
.\vcpkg install boost-range:x64-windows boost-range:x86-windows
.\vcpkg install boost-log:x64-windows boost-log:x86-windows
.\vcpkg install boost-property-tree:x64-windows boost-property-tree:x86-windows
.\vcpkg install boost-spirit:x64-windows boost-spirit:x86-windows
.\vcpkg install boost-uuid:x64-windows boost-uuid:x86-windows
.\vcpkg install boost-locale:x64-windows boost-locale:x86-windows
.\vcpkg install boost-iostreams:x64-windows boost-iostreams:x86-windows
  
.\vcpkg export ^
	poco:x64-windows poco:x86-windows ^
	protobuf:x64-windows protobuf:x86-windows ^
	gtest:x64-windows gtest:x86-windows ^
	ctemplate:x64-windows ctemplate:x86-windows ^
	boost-optional:x64-windows boost-optional:x86-windows ^
	boost-filesystem:x64-windows boost-filesystem:x86-windows ^
	boost-algorithm:x64-windows boost-algorithm:x86-windows ^
	boost-container:x64-windows boost-container:x86-windows ^
	boost-program-options:x64-windows boost-program-options:x86-windows ^
	boost-regex:x64-windows boost-regex:x86-windows ^
	boost-range:x64-windows boost-range:x86-windows ^
	boost-log:x64-windows boost-log:x86-windows ^
	boost-property-tree:x64-windows boost-property-tree:x86-windows ^
	boost-spirit:x64-windows boost-spirit:x86-windows ^
	boost-uuid:x64-windows boost-uuid:x86-windows ^
	boost-locale:x64-windows boost-locale:x86-windows ^
	boost-iostreams:x64-windows boost-iostreams:x86-windows ^
	--nuget --nuget-id=ThirdParty --nuget-version=1.4.0

downloads\tools\nuget-4.6.2-windows\nuget.exe install ThirdParty -Source %ROOT_FOLDER%\vcpkg -OutputDirectory ..\..\..\packages