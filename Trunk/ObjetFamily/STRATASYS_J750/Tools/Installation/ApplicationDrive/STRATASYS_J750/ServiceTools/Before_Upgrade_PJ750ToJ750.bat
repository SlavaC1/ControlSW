@echo off
SET Version=XP
VER | FINDSTR /IL "6.1." > NUL
IF %ERRORLEVEL% EQU 0 SET Version=7

IF %Version% EQU 7  (
 del "%PUBLIC%\Desktop\STRATASYS_PJ750.lnk"
)
IF %Version% EQU XP  (
 del "%allusersprofile%\Desktop\STRATASYS_PJ750.lnk"
)

MOVE C:\STRATASYS_PJ750 C:\STRATASYS_J750 
MOVE C:\STRATASYS_J750\STRATASYS_PJ750.exe C:\STRATASYS_J750\STRATASYS_J750.exe 	
	
	echo Update before upgrade was done
	echo msgbox "The update was done!" > %tmp%\tmp.vbs
cscript /nologo %tmp%\tmp.vbs
del %tmp%\tmp.vbs
	