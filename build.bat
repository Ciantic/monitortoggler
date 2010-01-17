@rem This is Microsoft Visual Studio 8 command line build script
@echo off
@if "%VCINSTALLDIR%" == "" call "%VS90COMNTOOLS%vsvars32.bat"

rem Easiest way to compile:
rem cl monitortoggler.c /link user32.lib

rem Second easiest way to compile (Os optimize size):
cl /Os src/monitortoggler.c /link user32.lib

rem Delete obj file, it's useless
del monitortoggler.obj