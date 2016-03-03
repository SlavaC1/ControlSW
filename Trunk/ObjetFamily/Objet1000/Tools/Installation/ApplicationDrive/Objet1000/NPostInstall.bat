@ECHO off
echo demo mode
CD %1 
copy .\Jungo\wdapi1040_32.dll .\wdapi1040.dll

rem echo Registering RF Reader DLL...
rem -----------------------------------------
rem CD %1\RF_Reader
rem CALL Register.bat