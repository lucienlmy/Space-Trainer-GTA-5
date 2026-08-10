@echo off
setlocal
cd /d "%~dp0"

echo [1/2] Generating VS2022 solution via Premake...
"Solution\external\premake\premake5.exe" vs2022
if errorlevel 1 exit /b 1

echo [2/2] Building Release x64 with MSBuild (no Visual Studio IDE)...
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "MSBUILD="
if exist "%VSWHERE%" (
  for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set "MSBUILD=%%i"
)
if not defined MSBUILD if exist "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe" set "MSBUILD=%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
if not defined MSBUILD if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe" set "MSBUILD=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe"

if not defined MSBUILD (
  echo.
  echo ERROR: MSBuild not found.
  echo Install "Build Tools for Visual Studio 2022" with C++ workload, then re-run this script.
  echo Download: https://aka.ms/vs/17/release/vs_BuildTools.exe
  echo Workload: Desktop development with C++ / MSVC v143 + Windows SDK
  exit /b 2
)

"%MSBUILD%" /m /p:Configuration=Release /p:Platform=x64 "Solution\Space.sln"
if errorlevel 1 exit /b 1

echo [post] Copying SpaceStuff data next to Space.asi...
set "OUT=Solution\source\_Build\bin\Release"
if not exist "%OUT%\SpaceStuff" mkdir "%OUT%\SpaceStuff"
xcopy /E /I /Y /Q "SpaceStuff\*" "%OUT%\SpaceStuff\" >nul
if errorlevel 1 (
  echo WARNING: failed to copy SpaceStuff folder.
) else (
  echo Space OK: %OUT%\SpaceStuff\  (needs PedList.xml etc. next to GTA5.exe)
)

echo.
echo Build OK: %OUT%\Space.asi
echo Install BOTH Space.asi AND the SpaceStuff folder into your GTA V root.
exit /b 0
