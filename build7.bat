@rem This is Microsoft Visual Studio 8 command line build script
@echo off
@if "%VCINSTALLDIR%" == "" call "%VS90COMNTOOLS%vsvars32.bat"

rem Restore settings application
rem cl src/restoremonitors7.c /link user32.lib

cl /nologo /O1 /c src/restoremonitors7.c
link /nologo restoremonitors7.obj user32.lib

rem Delete obj files
del restoremonitors7.obj