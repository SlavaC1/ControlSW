4/10/06

General:
========
The purpose of the 'ParametersExportAndImport' utility is to allow extraction of data from existing configuration files,
and to allow updating data in existing configuration files.

In order to add parameters which were created in the Eden last version, it is essential to use the 'ParametersExportAndImport' 
utility that was created with the latest version of the Eden software.

Notice: The 'ParametersExportAndImport' is suitable ONLY for parameters which have the same name as used to be.

PreUninstall:
=============
The 'PreUninstall' batch file copies the current config file and modes to a temporary directory

PostInstall:
============
A) Updates the new installed PreMachine_HQ Mode with the old PreMachine_HQ parameters
B) Updates the new installed PreMachine_HS Mode with the old PreMachine_HS parameters
C) Updates the new installed main Q2RT.cfg file with the old Per-Machine settings



Installation Instructions:
==========================
Please locate 'ParametersExportAndImport' directory under the 'ServiceTools' directory.

The 'PreUninstall.bat' batch file should be run BEFORE the new installation.
The 'PostInstall.bat' batch file should be run AFTER installing the new version.

Files:
======

MajorPerMachine.cfg - This file should contain all the parameters which are Per-Machine settings (i.e. machines settings
                      which are unique to this machine, but NOT HQ or HS. Example: 'Z_StartPrintPosition', 'PurgeXStart'). 
                      These parameters will be transferred to the new Q2RT.CFG


AddedParameters.cfg - This file should contain parameters which are ALREADY in the 'MajorPerMachine.cfg' but need
                      to force new values.
                      Example - UVSensorBypass: this parameter is in the 'MajorPerMachine.cfg' because we want to keep bypassing
                      or not, according to the current machine. Once we installed UV Sensor, we want to force this parameter
                      to 'False', so we put it in this file, with 'False' value.
         
           

AddedParams_HQ.cfg* - This file should contain new added parameters to the '~PerMachine_HQ.cfg' file, or old parameters 
                     (which are already in the '~PerMachine_HQ.cfg') for which we want to force new values


AddedParams_HS.cfg* - This file should contain new added parameters to the '~PerMachine_HS.cfg' file, or old parameters 
                     (which are already in the '~PerMachine_HS.cfg') for which we want to force new values


* New parameters which are in these files HAVE TO BE also in the new installed '~PerMachine_HS/HQ.cfg'