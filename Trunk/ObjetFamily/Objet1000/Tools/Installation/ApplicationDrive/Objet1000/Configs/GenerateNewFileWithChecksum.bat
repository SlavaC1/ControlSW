REM Make sure you have an updated ParamsExportAndImport.exe in the correct location. (it should include all parameters in its compilation.)
del q2rt.cfg.bak
copy q2rt.cfg q2rt.aaa
"..\..\..\Base\Tools\ParametersExportAndImport\ParametersExportAndImport\ParamsExportAndImport.exe" export q2rt.aaa q2rt.cfg 
del .bak q2rt.aaa
pause