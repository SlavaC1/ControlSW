@ECHO off

echo Update new parameters with old per machine parameters
rem -----------------------------------------------------
CD %1\ServiceTools\ParametersExportAndImport

CALL OnUpgradeUpdates.bat

rem %2=="1" - demo mode, 
IF "%2"=="0" (goto UPGRADE) else (goto DEMO)

:UPGRADE
echo Installing Hasp Driver...
start "" /wait ..\..\c2v\haspdinst.exe -i -cm -kp -fi -nomsg

echo Installing Jungo driver...
rem -----------------------------------------
CD %1\Jungo 
CALL UnRegisterJungo.bat

CALL RegisterJungo.bat
goto :END
:DEMO
echo demo mode
CD %1 
copy .\Jungo\wdapi1040_32.dll .\wdapi1040.dll
:END
rem echo Registering RF Reader DLL...
rem -----------------------------------------
rem CD %1\RF_Reader
rem CALL Register.bat

