@SET BDS=C:\Program Files\CodeGear\RAD Studio\5.0
@SET BDSCOMMONDIR=C:\Documents and Settings\All Users\Documents\RAD Studio\5.0
@SET FrameworkDir=C:\WINDOWS\Microsoft.NET\Framework\
@SET FrameworkVersion=v2.0.50727
@SET FrameworkSDKDir=

@ECHO Setting environment for using CodeGear RAD Studio tools

@SET PATH=%FrameworkDir%%FrameworkVersion%;%FrameworkSDKDir%;%PATH%

@pushd .
@cd Release
@impdef LicenseManagerOS.def LicenseManagerOS.dll
@popd

@VSTools\lib /machine:x86 /def:Release\LicenseManagerOS.def /out:LicenseManagerOS_VS.lib

@pause

