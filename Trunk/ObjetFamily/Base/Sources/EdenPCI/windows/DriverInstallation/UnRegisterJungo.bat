@devcon disable "@PCI\VEN_11AE&DEV_464E&SUBSYS_908010B5&REV_00\4&1A671D0C&0&38F0"
@devcon disable "@ROOT\JUNGO\0000"

@echo off
rem Removing old driver versions ...
wdreg -name KP_EDENP uninstall
wdreg -inf DATAPCI.inf uninstall
wdreg -inf windrvr6.inf uninstall

IF EXIST %windir%\system32\drivers\windrvr6.sys del %windir%\system32\drivers\windrvr6.sys
IF EXIST %windir%\system32\drivers\wdpnp.sys del %windir%\system32\drivers\wdpnp.sys
IF EXIST %windir%\system32\drivers\KP_EDENP.sys del %windir%\system32\drivers\KP_EDENP.sys

@devcon enable "@PCI\VEN_11AE&DEV_464E&SUBSYS_908010B5&REV_00\4&1A671D0C&0&38F0"
@devcon enable "@ROOT\JUNGO\0000"
