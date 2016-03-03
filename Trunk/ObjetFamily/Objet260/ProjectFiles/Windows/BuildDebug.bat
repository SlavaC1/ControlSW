@SET BDS=C:\Program Files\CodeGear\RAD Studio\5.0
@SET BDSCOMMONDIR=C:\Documents and Settings\All Users\Documents\RAD Studio\5.0
@SET FrameworkDir=C:\WINDOWS\Microsoft.NET\Framework\
@SET FrameworkVersion=v2.0.50727
@SET FrameworkSDKDir=

@ECHO Setting environment for using CodeGear RAD Studio tools

@SET PATH=%FrameworkDir%%FrameworkVersion%;%FrameworkSDKDir%;%PATH%

@call ForceVerUpdate.bat

@pushd .
@cd ..\..\..\..\SharedFiles\Qlib\LibraryProject\Windows
@msbuild QLibProject.cbproj /t:clean;build /p:config="Debug Build"
@popd

@msbuild Objet260.cbproj /t:clean;build /p:config="Debug Build"

@pause

