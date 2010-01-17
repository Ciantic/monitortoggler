@rem This is Microsoft Visual Studio 8 command line build script
@echo off
@if "%VCINSTALLDIR%" == "" call "%VS90COMNTOOLS%vsvars32.bat"

rem Easiest way to compile:
rem cl monitortoggler.c /link user32.lib

rem Smaller way to compile, I would like to know even smaller btw,
rem but then again I'm not just yet in mood of littering C code with 
rem calls to DLL imports...
cl /nologo /O1 /c src/monitortoggler.c
link /nologo monitortoggler.obj user32.lib

rem Delete obj file, it's useless
del monitortoggler.obj