@echo off

set CONFIGURATION=Release
rem set CONFIGURATION=Debug

set CURRENTFOLDER=VaultIC_1XX_%CONFIGURATION%
cd %CURRENTFOLDER%

rem MANUFACTURER password need to be specified below to continue
rem set VIC_100_PWD=
rem set VIC_150_PWD=

set MANUF_PASSWORD=%VIC_150_PWD%
rem set MANUF_PASSWORD=%VIC_100_PWD%

xcopy /y "..\..\..\build\windows\VaultIC_API_1XX\dist\Win32\%CONFIGURATION%\VaultIC_API_1XX.dll" .\
xcopy /y "..\..\..\lib\aardvark.dll" .\
cd ..

%CURRENTFOLDER%\VaultIC_1XX_SampleCode.exe %MANUF_PASSWORD% 
echo ----------------------------------------------------------------
echo.
%CURRENTFOLDER%\VaultIC_1XX_PersonaliseSampleCode.exe %MANUF_PASSWORD%
echo ----------------------------------------------------------------
echo.
%CURRENTFOLDER%\VaultIC_1XX_AuthenticationSampleCode.exe
echo ----------------------------------------------------------------
echo.
rem %CURRENTFOLDER%\VaultIC_1XX_FipsPersonaliseSampleCode.exe %MANUF_PASSWORD%
echo.
rem %CURRENTFOLDER%\VaultIC_1XX_FipsAuthenticationSampleCode.exe
echo.
%CURRENTFOLDER%\VaultIC_100_CountersSampleCode.exe %MANUF_PASSWORD% 
echo ----------------------------------------------------------------
echo.
cd %CURRENTFOLDER%
del "VaultIC_API_1XX.dll"
del "aardvark.dll"
cd ..

pause