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
IF "%PROCESSOR_ARCHITECTURE%"=="x86" (
SET OSType=32bit
SET BDS=C:\Program Files\CodeGear\RAD Studio\5.0
) ELSE (
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
@rem msbuild PowerFailHandler.cbproj /t:clean;build /p:config="Release Build"
@popd

@pushd .
@cd ..\..\..\..\SharedFiles\Qlib\LibraryProject\Windows
@msbuild QLibProject.cbproj /t:clean;build /p:config="Release Build"
rem @msbuild QLibProject.cbproj /t:make /p:config="Release Build"
@popd

@pushd .
@cd ..\..\..\..\..\..\Trunk\Raccoon\Raccoon\IDT-Lib\vaultic-api-release_1.2.0\src\build\windows\ValutIC_API_1_BCB
@msbuild VaultIC.cbproj /t:clean;build /p:config="Release"
@popd
@set ParametersExportAndImport=..\..\..\Base\Tools\ParametersExportAndImport
@xcopy Version.rc %ParametersExportAndImport% /Y

@pushd .
@cd %ParametersExportAndImport%
@rem msbuild ParamsExportAndImport.cbproj /t:clean;build /p:config="Release Build"
@popd

@rem msbuild Objet350.cbproj /t:clean;build /p:config="Emulation"

@pushd .
@cd ..\..\Runtime
@rem copy Objet350.exe Objet350_emulation.exe /Y
@rem del Objet350.exe
@popd
@msbuild Objet350.cbproj /t:clean;build /p:config="Release Build"
rem @msbuild Objet350.cbproj /t:make /p:config="Release Build"

@pause

