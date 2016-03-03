rem @ECHO off

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

rem Export from the temp Q2RT.cfg into the '~PerMachine_HQ.cfg'
rem ----------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe export ..\..\Modes\HighQualityModes\~PerMachine_HQ.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL2)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL2)

rem import the old '~PerMachine_HS' parameters into the temp Q2RT.cfg
rem -----------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe import ..\..\Backup\Modes\HighSpeedModes\~PerMachine_HS.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL3)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL3)

rem Export from the temp Q2RT.cfg into the '~PerMachine_HS.cfg'
rem ----------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe export ..\..\Modes\HighSpeedModes\~PerMachine_HS.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL4)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL4)


rem import the old '~PerMachine_DM' parameters into the temp Q2RT.cfg
rem ------------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe import ..\..\Backup\Modes\DigitalMaterialsModes\~PerMachine_DM.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL6)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL6)

rem Export from the temp Q2RT.cfg into the '~PerMachine_DM.cfg'
rem ----------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe export ..\..\Modes\DigitalMaterialsModes\~PerMachine_DM.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL7)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL7)


if exist ..\..\Backup\configs\q2rt.cfg.bak del ..\..\Backup\configs\q2rt.cfg.bak
if exist .\q2rt.cfg.bak del .\q2rt.cfg.bak

rem Major ~PerMachine 
rem -------------------------

rem Export the machine's PerMachine parameters to 'MajorPerMachine' file
rem --------------------------------------------------------------------------------------------

if not exist ..\..\Backup\Modes\DefaultModes start "" /wait ParamsExportAndImport_Prev.exe export MajorPerMachine.cfg ..\..\Backup\configs\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL5)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL5) 


if exist ..\..\Backup\Modes\DefaultModes start "" /wait ParamsExportAndImport.exe export MajorPerMachine.cfg ..\..\Backup\Modes\DefaultModes\~PerMachineDefault.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL5)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL5)



start "" /wait ParamsExportAndImport.exe import MajorPerMachine.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL5)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL5)

rem import the old 'PerMachine' parameters into the installed Q2RT.cfg
rem --------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe export ..\..\Modes\DefaultModes\~PerMachineDefault.cfg .\q2rt.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL5)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL5)

copy ..\..\Modes\DefaultModes\~PerMachineDefault.cfg ..\..\Modes\DefaultModes\~PerMachineDefaultTemp.cfg

rem export ~PerMachineDefault.cfg to ~PerMachineDefault.cfg to create updated checksum
rem --------------------------------------------------------------------------------------
start "" /wait ParamsExportAndImport.exe export ..\..\Modes\DefaultModes\~PerMachineDefaultTemp.cfg ..\..\Modes\DefaultModes\~PerMachineDefault.cfg \Q
IF ERRORLEVEL 1 (GOTO ERROR_LABEL5)
IF NOT ERRORLEVEL 0 (GOTO ERROR_LABEL5)
rem pause

rem in case no directory in the backup for DefaultModes:
rem ParamsExportAndImport_Prev.exe export MajorPerMachine.cfg q2rt_old.cfg
rem ParamsExportAndImport.exe import MajorPerMachine.cfg q2rt_new.cfg
rem ParamsExportAndImport.exe export ~PerMachineDefault.cfg q2rt_new.cfg


rem Maintenance Counters
rem ----------------------------
rem Copy the old 'Maintenance Counters' back...
rem --------------------------------------------------------
copy ..\..\Backup\configs\Maintenance.dat ..\..\configs\Maintenance.dat
copy ..\..\Backup\configs\Maintenance.bak ..\..\configs\Maintenance.bak

rem Copy the old 'SensorVacuum.txt' back...
rem --------------------------------------------------------
copy ..\..\Backup\configs\SensorVacuum.txt ..\..\configs\SensorVacuum.txt

rem Copy the old UV history file back
rem -----------------------------------------
copy "..\..\Backup\ServiceTools\UV Lamps\UV Lamps History.txt" "..\..\ServiceTools\UV Lamps\UV Lamps History.txt"

rem Copy the old Replaced Heads History file back - Recover from 'SHR' folder (in case of upgrade from '50.0.1.14' to '50.3')
rem ---------------------------------------------------------
if exist "..\..\Backup\ServiceTools\SHR" md "..\..\ServiceTools\Head Optimization Wizard"
copy "..\..\Backup\ServiceTools\SHR\Replaced Heads History.txt" "..\..\ServiceTools\Head Optimization Wizard\Replaced Heads History.txt"

rem Copy the old Replaced Heads History file back - Recover from 'Head Optimization Wizard' folder (in case of upgrade from '50.3' to '50.3')
rem ---------------------------------------------------------
if exist "..\..\Backup\ServiceTools\Head Optimization Wizard" md "..\..\ServiceTools\Head Optimization Wizard"
copy "..\..\Backup\ServiceTools\Head Optimization Wizard\Replaced Heads History.txt" "..\..\ServiceTools\Head Optimization Wizard\Replaced Heads History.txt"

if exist "..\..\Backup\configs\follow - up log.txt" copy "..\..\Backup\configs\follow - up log.txt" "..\..\configs\follow - up log.txt"




goto END

rem CleanUP
rem ----------
							
:ERROR_LABEL1
echo ***************************************************************************
echo Failed to import the old '~PerMachine_HQ' parameters into the temp Q2RT.cfg
goto ERROR_END
:ERROR_LABEL2
echo ***************************************************************************
echo Failed to Export from the temp Q2RT.cfg into the '~PerMachine_HQ.cfg
goto ERROR_END
:ERROR_LABEL3
echo ***************************************************************************
echo Failed to import the old '~PerMachine_HS' parameters into the temp Q2RT.cfg
goto ERROR_END
:ERROR_LABEL4
echo ***************************************************************************
echo Failed to Export from the temp Q2RT.cfg into the '~PerMachine_HS.cfg
goto ERROR_END
:ERROR_LABEL5
echo **********************************************************************************************
echo Failed to Export the machine's PerMachine parameters to 'PerMachineDefault' file
goto ERROR_END
:ERROR_LABEL6
echo ***************************************************************************
echo Failed to import the old '~PerMachine_DM' parameters into the temp Q2RT.cfg
goto ERROR_END
:ERROR_LABEL7
echo ***************************************************************************
echo Failed to Export from the temp Q2RT.cfg into the '~PerMachine_DM.cfg
goto ERROR_END
:ERROR_END
echo ERROR: %errorlevel%
echo Copying the old Configs and Modes directory back to the original directory
echo Fix the problem and try updating again
echo **********************************************************************************************
rem xcopy ..\..\Backup\*.* /s/e ..\..\*.*
:END
						
rem Delete all temporary files 
rem ---------------------------------
del .\q2rt.cfg
del .\q2rt.cfg.bak
del ..\..\Backup\*.* /S /Q
del /s ..\..\Backup  /Q
rd /s/q ..\..\Backup
del .\ParamsExportAndImport_Prev.exe /S /Q
del ..\..\Modes\DefaultModes\~PerMachineDefaultTemp.cfg  /S /Q

