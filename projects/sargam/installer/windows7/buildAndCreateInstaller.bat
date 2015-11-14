@echo off

if "%1"=="" goto error

REM clean
pushd "c:\Program Files (x86)\MSBuild\12.0\Bin\"
"C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild" /t:clean /p:configuration=RELEASE e:\code\CmakeRealisim\Realisim.sln

REM build
"C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild" /t:build /p:configuration=RELEASE e:\code\CmakeRealisim\Realisim.sln
popd


REM installer
echo %1
pushd "E:\Program Files (x86)\Inno Setup 5\"
ISCC.exe "/dMyAppVersion=%1" E:\code\realisim\projects\sargam\installer\windows7\windowsInstaller.iss
popd

move sargam.exe sargam_%1.exe
GOTO end

:error
echo needs the version as argument. ex: buildAndCreateInstaller.bat 0.5.3

:end