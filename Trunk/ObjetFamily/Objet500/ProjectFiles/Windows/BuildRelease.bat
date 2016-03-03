@echo off
SET OSVersion=Unknown
SET OSType=Unknown

VER | FINDSTR /L "5.1." > NUL
IF %ERRORLEVEL%==0 (
SET OSVersion=Windows XP
SET BDSCOMMONDIR=C:\Documents and Settings\All Users\Documents\RAD Studio\5.0
goto OS_TYPE_DETECT
)

VER | FINDSTR /L "6.1." > NUL
IF %ERRORLEVEL%==0 (
SET OSVersion=Windows 7
SET BDSCOMMONDIR=C:\Users\Public\Public Documents\RAD Studio\5.0
goto OS_TYPE_DETECT
)

:OS_TYPE_DETECT
IF "%PROCESSOR_ARCHITECTURE%"=="x86" and "%PROCESSOR_ARCHITEW6432%"=="" (
		SET OSType=32bit
		SET BDS=C:\Program Files\CodeGear\RAD Studio\5.0
)ELSE(
SET OSType=64bit
SET BDS=C:\Program Files ^(x86^)\CodeGear\RAD Studio\5.0
)

@SET FrameworkDir=C:\WINDOWS\Microsoft.NET\Framework\
@SET FrameworkVersion=v2.0.50727
@SET FrameworkSDKDir=

@ECHO Operating system: %OSVersion% %OSType% %PROCESSOR_ARCHITECTURE%
@ECHO Setting environment for using CodeGear RAD Studio 2007 tools

@SET PATH=%FrameworkDir%%FrameworkVersion%;%FrameworkSDKDir%;%PATH%

@call ForceVerUpdate.bat

@pushd .
@cd ..\..\Tools\PowerFailHandler
@msbuild PowerFailHandler.cbproj /t:clean;build /p:config="Release Build" 
@popd

@pushd .
@cd ..\..\..\..\SharedFiles\Qlib\LibraryProject\Windows
@msbuild QLibProject.cbproj /t:clean;build /p:config="Release Build" 
@popd

@set ParametersExportAndImport=..\..\..\Base\Tools\ParametersExportAndImport
@xcopy Version.rc %ParametersExportAndImport% /Y

@pushd .
@cd %ParametersExportAndImport%
@msbuild ParamsExportAndImport.cbproj /t:clean;build /p:config="Release Build" 
@popd

rem @msbuild Objet500.cbproj /t:clean;build /p:config="Emulation" 

@pushd .
@cd ..\..\Runtime
rem @copy Objet500.exe Objet500_emulation.exe /Y
rem @del Objet500.exe
@popd

@msbuild Objet500.cbproj /t:clean;build /p:config="Release Build" 

@pause

