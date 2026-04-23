@echo off
setlocal

set "VSDEV=D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
if not exist "%VSDEV%" (
  echo [ERROR] VsDevCmd not found: %VSDEV%
  exit /b 1
)

call "%VSDEV%" -arch=x64
if errorlevel 1 exit /b 1

set "TARGET=%~1"
if "%TARGET%"=="" set "TARGET=exp6"

if not exist "%~dp0%TARGET%\main.cpp" (
  echo [ERROR] Missing source: %TARGET%\main.cpp
  exit /b 1
)

set "EXE=%TARGET%.exe"
if /I "%TARGET%"=="exp6" (
  set "LINKLIBS=opengl32.lib glu32.lib"
) else (
  set "LINKLIBS="
)

pushd "%~dp0%TARGET%"
cl /nologo /std:c++17 /EHsc /MD /D_AFXDLL /Fe:%EXE% main.cpp /link /NOLOGO /SUBSYSTEM:WINDOWS %LINKLIBS%
if errorlevel 1 (
  popd
  exit /b 1
)
popd

echo Build complete: %TARGET%\%EXE%
