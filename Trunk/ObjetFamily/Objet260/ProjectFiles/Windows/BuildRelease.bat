@echo off
@SET BDS=C:\Program Files\CodeGear\RAD Studio\5.0
@SET BDSCOMMONDIR=C:\Documents and Settings\All Users\Documents\RAD Studio\5.0
@SET FrameworkDir=C:\WINDOWS\Microsoft.NET\Framework\
@SET FrameworkVersion=v2.0.50727
@SET FrameworkSDKDir=

@ECHO Setting environment for using CodeGear RAD Studio tools

@SET PATH=%FrameworkDir%%FrameworkVersion%;%FrameworkSDKDir%;%PATH%

@call ForceVerUpdate.bat

@pushd .
@cd ..\..\Tools\PowerFailHandler
@msbuild PowerFailHandler.cbproj /t:clean;build /p:config="Release Build"
@popd

@pushd .
@cd ..\..\..\..\SharedFiles\Qlib\LibraryProject\Windows
@msbuild QLibProject.cbproj /t:clean;build /p:config="Release Build"
rem @msbuild QLibProject.cbproj /t:make /p:config="Release Build"
@popd

@set ParametersExportAndImport=..\..\..\Base\Tools\ParametersExportAndImport
@xcopy Version.rc %ParametersExportAndImport% /Y

@pushd .
@cd %ParametersExportAndImport%
@msbuild ParamsExportAndImport.cbproj /t:clean;build /p:config="Release Build"
@popd

@msbuild Objet260.cbproj /t:clean;build /p:config="Emulation"

@pushd .
@cd ..\..\Runtime
@copy Objet260.exe Objet260_emulation.exe /Y
@del Objet260.exe
@popd
@msbuild Objet260.cbproj /t:clean;build /p:config="Release Build"
rem @msbuild Objet260.cbproj /t:make /p:config="Release Build"

@pause

