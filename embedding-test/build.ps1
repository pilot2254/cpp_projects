# Builds payload.dll, then app.exe with the DLL baked in. Only build\app.exe needs to be shipped.
$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot
New-Item -ItemType Directory -Force build | Out-Null

arkari++ -std=c++20 -O2 -mllvm -irobf-indbr -mllvm -irobf-icall -mllvm -irobf-indgv -mllvm -irobf-cse -mllvm -irobf-fla -mllvm -irobf-cie -mllvm -irobf-cfe -shared dll\main.cpp -o build\payload.dll
if ($LASTEXITCODE) { exit $LASTEXITCODE }

# Converts build\payload.dll into a hardcoded hex array header, build\payload.h,
# which exe\main.cpp #includes directly.
# Absolute paths here: ReadAllBytes resolves relative paths against the process's
# .NET working directory, which doesn't always track Set-Location, so a relative
# path can silently resolve one level off from where you'd expect.
& "$PSScriptRoot\bin2header.ps1" -InputFile "$PSScriptRoot\build\payload.dll" -OutputFile "$PSScriptRoot\build\payload.h" -ArrayName dll_bytes
if ($LASTEXITCODE) { exit $LASTEXITCODE }

arkari++ -std=c++20 -O2 -Ibuild exe\main.cpp -o build\app.exe
if ($LASTEXITCODE) { exit $LASTEXITCODE }

Write-Host "Built build\app.exe"
