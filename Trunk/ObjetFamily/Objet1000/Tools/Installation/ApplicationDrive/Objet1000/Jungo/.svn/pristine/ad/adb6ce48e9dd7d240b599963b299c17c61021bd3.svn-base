@echo off
rem Installing Data PCI driver (Jungo) 10.4 ...
cd %1
certmgr.exe /add Objet.cer /s /r localMachine root
wdreg -rescan PCI
wdreg -inf .\windrvr6.inf install
wdreg -inf .\DataPCI.inf install
copy .\KP_EDENP.sys %windir%\Sysnative\drivers\. /y
wdreg -name KP_EDENP install
copy .\wdapi1040.dll %windir%\Sysnative\. /y
copy .\wdapi1040_32.dll %windir%\SysWOW64\wdapi1040.dll /y
