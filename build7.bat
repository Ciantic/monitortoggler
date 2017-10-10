@rem This is Microsoft Visual Studio 14 command line build script
@echo off

:: 64bit build
::@if "%VCINSTALLDIR%" == "" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64

:: 32bit build
@if "%VCINSTALLDIR%" == "" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86

rem Restore settings application
rem cl src/restoremonitors7.c /link user32.lib

cl /nologo /O1 /EHsc /c src/restoremonitors7.cpp
link /nologo restoremonitors7.obj user32.lib

rem Delete obj files
del restoremonitors7.obj
