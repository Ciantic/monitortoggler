@echo off 
setlocal enableextensions 
rem So you want to *toggle* between two profiles?
rem 
rem Remember to change "default.sdc" and "klooni.sdc" to ones
rem you want to toggle.

for /f "tokens=*" %%a in ( 
	'restoremonitors7.exe -equal default.sdc' 
) do ( 
	set toggled=%%a 
) 
if "%toggled%" == "1 " restoremonitors7.exe klooni.sdc
if NOT "%toggled%" == "1 " restoremonitors7.exe default.sdc