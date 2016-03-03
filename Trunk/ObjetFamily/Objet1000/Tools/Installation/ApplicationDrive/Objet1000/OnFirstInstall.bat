echo Installing hasp Driver:
start "" /wait c2v\haspdinst.exe -i -cm -kp -fi -nomsg

echo Installing Jungo driver...
rem -----------------------------------------
CD %1\Jungo 
CALL RegisterJungo.bat
