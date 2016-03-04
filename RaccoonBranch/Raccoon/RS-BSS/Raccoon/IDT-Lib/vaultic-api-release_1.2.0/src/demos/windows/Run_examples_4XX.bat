@echo off

set CONFIGURATION=Release
rem set CONFIGURATION=Debug

set CURRENTFOLDER=VaultIC_4XX_%CONFIGURATION%
cd %CURRENTFOLDER%

rem MANUFACTURER password need to be specified below to continue
set VIC_4XX_PWD=

set MANUF_PASSWORD=%VIC_4XX_PWD%

xcopy /y "..\..\..\build\windows\VaultIC_API_4XX\dist\Win32\%CONFIGURATION%\VaultIC_API_4XX.dll" .\
xcopy /y "..\..\..\lib\aardvark.dll" .\
cd ..

%CURRENTFOLDER%\VaultIC_4XX_SampleCode.exe %MANUF_PASSWORD% 
echo ----------------------------------------------------------------
echo.
%CURRENTFOLDER%\VaultIC_4XX_Reinitialization.exe %MANUF_PASSWORD%
echo ----------------------------------------------------------------
echo.
rem %CURRENTFOLDER%\VaultIC_4XX_KeyWrapping.exe %MANUF_PASSWORD%
echo ----------------------------------------------------------------
echo.
rem "%CURRENTFOLDER%\VaultIC_4XX_KeyWrapping - RSA.exe" %MANUF_PASSWORD%
echo ----------------------------------------------------------------
echo.
rem must have a Vautlic with patch
rem %CURRENTFOLDER%\VaultIC_4XX_KeyDerivation.exe %MANUF_PASSWORD%
echo ----------------------------------------------------------------
echo.
%CURRENTFOLDER%\VaultIC_4XX_Signature.exe %MANUF_PASSWORD%
echo ----------------------------------------------------------------


cd %CURRENTFOLDER%
del "VaultIC_API_4XX.dll"
del "aardvark.dll"
cd ..

pause