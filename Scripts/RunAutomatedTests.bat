@echo off
setlocal enabledelayedexpansion

set PluginName=Arcana

:: Check for the standalone argument
set "RUN_MODE=AutomationCommandlet"
if /i "%~1"=="-standalone" (
    set "RUN_MODE=game"
)

:: Get the directory of the current script (Plugins/PluginName/Scripts)
set "SCRIPT_DIR=%~dp0"

:: Navigate up to find the project root
cd /d "%SCRIPT_DIR%\..\..\.."
set "PROJECT_ROOT=%CD%"

:: Find the .uproject file
set "UPROJECT_PATH="
for %%F in ("%PROJECT_ROOT%\*.uproject") do (
    set "UPROJECT_PATH=%%F"
    goto :FoundProject
)

:FoundProject
if "%UPROJECT_PATH%"=="" (
    echo [ERROR] No .uproject file found in %PROJECT_ROOT%
    pause
    exit /b 1
)

:: Find the Unreal Engine installation directory via registry
set "ENGINE_PATH="
for /f "tokens=2*" %%A in ('reg query "HKLM\SOFTWARE\EpicGames\Unreal Engine" /s /v "InstalledDirectory" 2^>nul') do (
    set "ENGINE_PATH=%%B"
)

if "%ENGINE_PATH%"=="" (
    echo [ERROR] Unreal Engine installation directory not found in registry.
    pause
    exit /b 1
)

set "RUNUAT_PATH=%ENGINE_PATH%\Engine\Build\BatchFiles\RunUAT.bat"

echo Project Found: %UPROJECT_PATH%
echo Engine Found:  %ENGINE_PATH%
echo Run Mode:     %RUN_MODE%
echo Running Automated Tests...

:: Execute the tests
call "%RUNUAT_PATH%" RunUnreal -project="%UPROJECT_PATH%" -scriptargs="-RunTest=%PluginName% -NullRHI -NoSound -%RUN_MODE%"

pause
