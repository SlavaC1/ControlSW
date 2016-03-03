REM Make sure you have an updated ParamsExportAndImport.exe in the correct location. (it should include all parameters in its compilation.)
del ~PerMachineDefault.cfg.bak
copy ~PerMachineDefault.cfg ~PerMachineDefault.aaa
"..\..\..\..\Base\Tools\ParametersExportAndImport\ParametersExportAndImport\ParamsExportAndImport.exe" export ~PerMachineDefault.aaa ~PerMachineDefault.cfg 
del .bak ~PerMachineDefault.aaa
pause