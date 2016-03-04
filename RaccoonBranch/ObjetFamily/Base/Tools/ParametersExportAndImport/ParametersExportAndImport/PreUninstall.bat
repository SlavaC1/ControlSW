@ECHO off

rem ### Copy the 'Configs' and 'Modes' directories to temporary location
xcopy ..\..\configs\*.* \SavedEden\configs\*.* /s /e /v
xcopy ..\..\Modes\*.* \SavedEden\Modes\*.* /s /e /v
