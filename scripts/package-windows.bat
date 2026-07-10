@echo off
setlocal

:: Find MSYS2
set "MSYS2_PATH=C:\msys64"
if not exist "%MSYS2_PATH%\usr\bin\bash.exe" (
    echo [ERROR] MSYS2 not found at %MSYS2_PATH%.
    echo Please install MSYS2 or update the MSYS2_PATH variable.
    exit /b 1
)

echo [INFO] Launching MSYS2 UCRT64 Packaging Process...
"%MSYS2_PATH%\usr\bin\bash.exe" --login -c "export MSYSTEM=UCRT64; export PATH=/ucrt64/bin:/usr/bin:$PATH; cd $(cygpath -u '%CD%') && bash scripts/package-windows.sh"

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Packaging failed.
    exit /b %ERRORLEVEL%
)

echo [SUCCESS] Packaging script completed.
