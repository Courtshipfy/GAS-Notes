@echo off
setlocal

set "UE_BUILD=D:\UE_5.8\Engine\Build\BatchFiles\Build.bat"
set "PROJECT_FILE=%~dp0AbilitiesLab.uproject"
set "TARGET_NAME=AbilitiesLabEditor"

if not exist "%UE_BUILD%" (
    echo [ERROR] Unreal Engine build script was not found:
    echo         %UE_BUILD%
    goto :failed
)

if not exist "%PROJECT_FILE%" (
    echo [ERROR] Project file was not found:
    echo         %PROJECT_FILE%
    goto :failed
)

echo Building %TARGET_NAME%...
echo.

call "%UE_BUILD%" %TARGET_NAME% Win64 Development "%PROJECT_FILE%" -WaitMutex -NoHotReloadFromIDE

if errorlevel 1 goto :failed

echo.
echo ========================================
echo Build succeeded.
echo ========================================
goto :finished

:failed
echo.
echo ========================================
echo Build failed. Check the errors above.
echo ========================================
if /I not "%~1"=="--no-pause" pause
exit /b 1

:finished
if /I not "%~1"=="--no-pause" pause
exit /b 0
