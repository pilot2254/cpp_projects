# Converts a binary file into a C array header (like `xxd -i`), but in PowerShell.
# Usage: .\bin2header.ps1 <input-bin> <output-header> <array-name>
param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile,
    [Parameter(Mandatory = $true)][string]$ArrayName
)
$ErrorActionPreference = "Stop"

$bytes = [IO.File]::ReadAllBytes($InputFile)

$sb = [System.Text.StringBuilder]::new()
[void]$sb.AppendLine("// Auto-generated from $InputFile - do not edit by hand.")
[void]$sb.AppendLine("#pragma once")
[void]$sb.AppendLine("static const unsigned char $ArrayName[] = {")

$hex = [string[]]::new($bytes.Length)
for ($i = 0; $i -lt $bytes.Length; $i++) {
    $hex[$i] = "0x{0:x2}" -f $bytes[$i]
}

# Wrap at 16 bytes per line so the file is readable instead of one giant line.
for ($i = 0; $i -lt $hex.Length; $i += 16) {
    $end = [Math]::Min($i + 16, $hex.Length) - 1
    [void]$sb.AppendLine("    " + ($hex[$i..$end] -join ", ") + ",")
}

[void]$sb.AppendLine("};")
[void]$sb.AppendLine("static const unsigned int ${ArrayName}_len = $($bytes.Length);")

[IO.File]::WriteAllText($OutputFile, $sb.ToString())
Write-Host "Wrote $OutputFile ($($bytes.Length) bytes)"

# thx claude <3
