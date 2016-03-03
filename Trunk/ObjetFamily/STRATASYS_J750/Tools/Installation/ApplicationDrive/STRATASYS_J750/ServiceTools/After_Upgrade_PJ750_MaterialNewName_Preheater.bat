@echo off
	Set scut=C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Objet
	cd /d %scut%
	del /s /q /f "STRATASYS_PJ750.lnk"
	del "C:\STRATASYS_J750\Configs\Material batch number.htm"
	del "C:\STRATASYS_J750\ServiceTools\Recent Maintenance Actions Performed\RecentActions.htm"
	del "C:\STRATASYS_J750\Modes\ModelDefaultMaterials\DurusWhite.cfg"
	del "C:\STRATASYS_J750\Modes\ModelDefaultMaterials\DurusWhite.ref"
	del "C:\STRATASYS_J750\Modes\ModelDefaultMaterials\RGD525.cfg"
	del "C:\STRATASYS_J750\Modes\ModelDefaultMaterials\RGD525.ref"
	del "C:\STRATASYS_J750\Modes\ModelDefaultMaterials\TangoBlack.cfg"
	del "C:\STRATASYS_J750\Modes\ModelDefaultMaterials\TangoBlack.ref"
	del "C:\STRATASYS_J750\Modes\ModelDefaultMaterials\TangoGrey.cfg"
	del "C:\STRATASYS_J750\Modes\ModelDefaultMaterials\TangoGrey.ref"

	del "C:\STRATASYS_J750\Modes\ModelHMMaterials\DurusWhite_HM.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHMMaterials\DurusWhite_HM.ref"
	del "C:\STRATASYS_J750\Modes\ModelHMMaterials\RGD525_HM.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHMMaterials\RGD525_HM.ref"
	del "C:\STRATASYS_J750\Modes\ModelHMMaterials\TangoBlack_HM.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHMMaterials\TangoBlack_HM.ref"
	del "C:\STRATASYS_J750\Modes\ModelHMMaterials\TangoGrey_HM.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHMMaterials\TangoGrey_HM.ref"

	del "C:\STRATASYS_J750\Modes\ModelHQMaterials\DurusWhite_HQ.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHQMaterials\DurusWhite_HQ.ref"
	del "C:\STRATASYS_J750\Modes\ModelHQMaterials\RGD525_HQ.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHQMaterials\RGD525_HQ.ref"
	del "C:\STRATASYS_J750\Modes\ModelHQMaterials\TangoBlack_HQ.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHQMaterials\TangoBlack_HQ.ref"

	del "C:\STRATASYS_J750\Modes\ModelHSMaterials\DurusWhite_HS.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHSMaterials\DurusWhite_HS.ref"
	del "C:\STRATASYS_J750\Modes\ModelHSMaterials\RGD525_HS.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHSMaterials\RGD525_HS.ref"
	del "C:\STRATASYS_J750\Modes\ModelHSMaterials\TangoBlack_HS.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHSMaterials\TangoBlack_HS.ref"
	del "C:\STRATASYS_J750\Modes\ModelHSMaterials\TangoGrey_HS.cfg"
	del "C:\STRATASYS_J750\Modes\ModelHSMaterials\TangoGrey_HS.ref"


	reg delete "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Stratasys Ltd.\STRATASYS_PJ750" /f
    setlocal enableextensions disabledelayedexpansion
     set "search=1300,1300,1300,1300,1300,1300,1300,1300,700,700,700,700,700"
    set "replace=1520,1520,1410,1410,1410,1410,1410,1410,780,780,780,780,780"

    set "textFile=C:\STRATASYS_J750\Modes\HighMixModes\~PerMachine_HM.cfg"

    for /f "delims=" %%i in ('type "%textFile%" ^& break ^> "%textFile%" ') do (
        set "line=%%i"
        setlocal enabledelayedexpansion
        set "line=!line:%search%=%replace%!"
        >>"%textFile%" echo(!line!
        endlocal
    )
	set "search=1300,1300,1300,1300,1300,1300,1300,1300,700,700,700,700,700"
    set "replace=1520,1520,1410,1410,1410,1410,1410,1410,780,780,780,780,780"

    set "textFile=C:\STRATASYS_J750\Modes\HighQualityModes\~PerMachine_HQ.cfg"

    for /f "delims=" %%i in ('type "%textFile%" ^& break ^> "%textFile%" ') do (
        set "line=%%i"
        setlocal enabledelayedexpansion
        set "line=!line:%search%=%replace%!"
        >>"%textFile%" echo(!line!
        endlocal
    )
	set "search=1300,1300,1300,1300,1300,1300,1300,1300,700,700,700,700,700"
    set "replace=1520,1520,1410,1410,1410,1410,1410,1410,780,780,780,780,780"

    set "textFile=C:\STRATASYS_J750\Modes\HighSpeedModes\~PerMachine_HS.cfg"

    for /f "delims=" %%i in ('type "%textFile%" ^& break ^> "%textFile%" ') do (
        set "line=%%i"
        setlocal enabledelayedexpansion
        set "line=!line:%search%=%replace%!"
        >>"%textFile%" echo(!line!
        endlocal
    )
	
	echo Update parameters
	echo msgbox "The update was done!" > %tmp%\tmp.vbs
cscript /nologo %tmp%\tmp.vbs
del %tmp%\tmp.vbs
	