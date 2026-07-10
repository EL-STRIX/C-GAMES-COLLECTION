$ErrorActionPreference = "Stop"

$MSYS2_PATH = "C:\msys64"
$BASH_EXE = Join-Path $MSYS2_PATH "usr\bin\bash.exe"

if (-Not (Test-Path $BASH_EXE)) {
    Write-Error "[ERROR] MSYS2 not found at $MSYS2_PATH. Please install MSYS2 or update the path."
    exit 1
}

$CurrentDir = Get-Location
$CurrentDirStr = $CurrentDir.Path.Replace('\', '/')

Write-Host "[INFO] Launching MSYS2 UCRT64 Packaging Process..." -ForegroundColor Cyan

# Use bash to translate path and execute the script inside UCRT64
$CmdArgs = @(
    "--login",
    "-c",
    "export MSYSTEM=UCRT64; export PATH=/ucrt64/bin:/usr/bin:`$PATH; cd `$(cygpath -u '$CurrentDirStr') && bash scripts/package-windows.sh"
)

& $BASH_EXE $CmdArgs

if ($LASTEXITCODE -ne 0) {
    Write-Error "[ERROR] Packaging failed with exit code $LASTEXITCODE."
    exit $LASTEXITCODE
}

Write-Host "[SUCCESS] Packaging script completed." -ForegroundColor Green
