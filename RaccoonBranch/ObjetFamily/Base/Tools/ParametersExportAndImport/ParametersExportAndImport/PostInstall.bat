@ECHO off

rem ### Delete the old backup file (if existed)
if exist ..\..\configs\q2rt.cfg.bak del ..\..\configs\q2rt.cfg.bak

rem ### Make a temp copy of the installed Q2RT.cfg
copy ..\..\configs\Q2RT.cfg .\



                            rem ### High Quality ~PerMachine ###
  

rem ### import the old '~PerMachine_HQ' parameters into the temp Q2RT.cfg
ParamsExportAndImport.exe import \SavedEden\Modes\HighQualityModes\~PerMachine_HQ.cfg .\q2rt.cfg

rem ### import new added '~PerMachine_HQ' parameters into the temp Q2RT.cfg
ParamsExportAndImport.exe import .\AddedParams_HQ.cfg .\q2rt.cfg

rem ### Export from the temp Q2RT.cfg into the '~PerMachine_HQ.cfg'
ParamsExportAndImport.exe export ..\..\Modes\HighQualityModes\~PerMachine_HQ.cfg .\q2rt.cfg



                            rem ### High Speed ~PerMachine ###
  

rem ### import the old '~PerMachine_HS' parameters into the temp Q2RT.cfg
ParamsExportAndImport.exe import \SavedEden\Modes\HighSpeedModes\~PerMachine_HS.cfg .\q2rt.cfg

rem ### import new added '~PerMachine_HS' parameters into the temp Q2RT.cfg
ParamsExportAndImport.exe import .\AddedParams_HS.cfg .\q2rt.cfg

rem ### Export from the temp Q2RT.cfg into the '~PerMachine_HS.cfg'
ParamsExportAndImport.exe export ..\..\Modes\HighSpeedModes\~PerMachine_HS.cfg .\q2rt.cfg



                            rem ### Major ~PerMachine ###


rem ### Export the machine's PerMachine parameters to 'MajorPerMachine' file
ParamsExportAndImport.exe export MajorPerMachine.cfg \SavedEden\configs\q2rt.cfg

rem ### import the old 'PerMachine' parameters into the installed Q2RT.cfg
ParamsExportAndImport.exe import MajorPerMachine.cfg ..\..\configs\q2rt.cfg

rem ### import new 'PerMachine' parameters into the installed Q2RT.cfg
ParamsExportAndImport.exe import AddedParameters.cfg ..\..\configs\q2rt.cfg



                            rem ### Maintenance Counters ###


rem ### Copy the old 'Maintenance Counters' back...
copy \SavedEden\configs\Maintenance.dat ..\..\configs\Maintenance.dat
copy \SavedEden\configs\Maintenance.bak ..\..\configs\Maintenance.bak



                            rem ### CleanUP ###


rem ### Delete all temporary files 
del .\q2rt.cfg
del .\q2rt.cfg.bak
rd \SavedEden /S /Q


