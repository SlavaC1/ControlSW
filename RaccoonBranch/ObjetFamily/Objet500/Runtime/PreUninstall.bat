@ECHO off

IF "%2"=="0" (goto UPGRADE_or_DEMO) else (goto UN_INSTALL)

:UPGRADE_or_DEMO
echo demo mode
goto :END

:UN_INSTALL
echo UnInstalling Jungo driver...
rem -----------------------------------------
CD %1\Jungo 
CALL UnRegisterJungo.bat

:END

rem pause