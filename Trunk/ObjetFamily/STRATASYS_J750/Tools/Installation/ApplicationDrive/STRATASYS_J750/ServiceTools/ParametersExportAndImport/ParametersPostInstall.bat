@ECHO off

rem Delete the old backup file (if existed)
rem ------------------------------------------------
if exist ..\..\configs\q2rt.cfg.bak del ..\..\configs\q2rt.cfg.bak

rem Make a temp copy of the installed Q2RT.cfg
rem --------------------------------------------------------
copy ..\..\configs\Q2RT.cfg .\

rem pause

rem High Quality ~PerMachine
rem ----------------------------------
  
rem import the old '~PerMachine_HQ' parameters into the temp Q2RT.cfg
rem ------------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe import ..\..\Backup\Modes\HighQualityModes\~PerMachine_HQ.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL1)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL1)

rem import new added '~PerMachine_HQ' parameters into the temp Q2RT.cfg
rem -----------------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe import .\AddedParams_HQ.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL2)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL2)

rem Export from the temp Q2RT.cfg into the '~PerMachine_HQ.cfg'
rem ----------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe export ..\..\Modes\HighQualityModes\~PerMachine_HQ.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL3)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL3)

rem pause

rem High Speed ~PerMachine
rem ---------------------------------

rem import the old '~PerMachine_HS' parameters into the temp Q2RT.cfg
rem -----------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe import ..\..\Backup\Modes\HighSpeedModes\~PerMachine_HS.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL4)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL4)

rem import new added '~PerMachine_HS' parameters into the temp Q2RT.cfg
rem ----------------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe import .\AddedParams_HS.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL5)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL5)

rem Export from the temp Q2RT.cfg into the '~PerMachine_HS.cfg'
rem ----------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe export ..\..\Modes\HighSpeedModes\~PerMachine_HS.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL6)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL6)
rem pause


rem Digital Material ~PerMachine
rem ---------------------------------

rem import the old '~PerMachine_DM' parameters into the temp Q2RT.cfg
rem -----------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe import ..\..\Backup\Modes\DigitalMaterialsModes\~PerMachine_DM.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL7)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL7)

rem import new added '~PerMachine_DM' parameters into the temp Q2RT.cfg
rem ----------------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe import .\AddedParams_DM.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL8)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL8)

rem Export from the temp Q2RT.cfg into the '~PerMachine_DM.cfg'
rem ----------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe export ..\..\Modes\DigitalMaterialsModes\~PerMachine_DM.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL9)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL9)
rem pause


rem Major ~PerMachine 
rem -------------------------

rem Export the machine's PerMachine parameters to 'MajorPerMachine' file
rem --------------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe export MajorPerMachine.cfg ..\..\Backup\configs\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL10)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL10)

rem import the old 'PerMachine' parameters into the installed Q2RT.cfg
rem --------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe import MajorPerMachine.cfg ..\..\configs\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL11)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL11)

rem import new 'PerMachine' parameters into the installed Q2RT.cfg
rem -----------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe import AddedParameters.cfg ..\..\configs\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL12)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL12)
rem pause


rem Maintenance Counters
rem ----------------------------
rem Copy the old 'Maintenance Counters' back
rem -----------------------------------------------------
copy ..\..\Backup\configs\Maintenance.dat ..\..\configs\Maintenance.dat
copy ..\..\Backup\configs\Maintenance.bak ..\..\configs\Maintenance.bak

rem SensorVacuum.txt file
rem ------------------------------
rem Copy the old 'SensorVacuum.txt' with the calibrated values back
rem ----------------------------------------------------------------------------------
copy ..\..\Backup\configs\SensorVacuum.txt ..\..\configs\SensorVacuum.txt


rem UV history file
rem ---------------
rem Copy the old 'UV history file back
rem ----------------------------------
copy "..\..\Backup\ServiceTools\UV Lamps\UV Lamps History.txt" "..\..\ServiceTools\UV Lamps\UV Lamps History.txt"
                          


rem pause
goto END

rem CleanUP
rem ----------
							
:ERROR_LABEL1
echo ***************************************************************************
echo Failed to import the old '~PerMachine_HQ' parameters into the temp Q2RT.cfg
goto ERROR_END
:ERROR_LABEL2
echo ***************************************************************************
echo Failed to import new added '~PerMachine_HQ' parameters into the temp Q2RT.cfg
goto ERROR_END
:ERROR_LABEL3
echo ***************************************************************************
echo Failed to Export from the temp Q2RT.cfg into the '~PerMachine_HQ.cfg
goto ERROR_END
:ERROR_LABEL4
echo ***************************************************************************
echo Failed to import the old '~PerMachine_HS' parameters into the temp Q2RT.cfg
goto ERROR_END
:ERROR_LABEL5
echo ***************************************************************************
echo Failed to import new added '~PerMachine_HS' parameters into the temp Q2RT.cfg
goto ERROR_END
:ERROR_LABEL6
echo ***************************************************************************
echo Failed to Export from the temp Q2RT.cfg into the '~PerMachine_HS.cfg
goto ERROR_END
:ERROR_LABEL7
echo ***************************************************************************
echo Failed to import the old '~PerMachine_DM' parameters into the temp Q2RT.cfg
goto ERROR_END
:ERROR_LABEL8
echo ***************************************************************************
echo Failed to import new added '~PerMachine_DM' parameters into the temp Q2RT.cfg
goto ERROR_END
:ERROR_LABEL9
echo ***************************************************************************
echo Failed to Export from the temp Q2RT.cfg into the '~PerMachine_DM.cfg
goto ERROR_END
:ERROR_LABEL10
echo ***************************************************************************
echo Failed to Export the machine's PerMachine parameters to 'MajorPerMachine' file
goto ERROR_END
:ERROR_LABEL11
echo ***************************************************************************
echo Failed to import the old 'PerMachine' parameters into the installed Q2RT.cfg
goto ERROR_END
:ERROR_LABEL12
echo ***************************************************************************
echo Failed to import new 'PerMachine' parameters into the installed Q2RT.cfg
goto ERROR_END
:ERROR_END
echo ERROR: %errorlevel%
echo Copying the old Configs and Modes directory back to the original directory
echo Fix the problem and try updating again
echo ***************************************************************************
xcopy ..\..\Backup\*.* /s/e ..\..\*.*
pause
:END
							
rem Delete all temporary files 
rem ---------------------------------
del .\q2rt.cfg
del .\q2rt.cfg.bak
rem pause

