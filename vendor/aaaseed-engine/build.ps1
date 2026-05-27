param(
    [string]$Config,
    [string]$Action = "Build",
    [switch]$Last
)

# Determine the path for storing the last configuration
$lastConfigFile = "$env:AAAOut\last_config.txt"

# If -Last is used, read the last configuration
if ($Last) {
    if (Test-Path $lastConfigFile) {
        $Config = Get-Content $lastConfigFile
    } else {
        Write-Host "No last configuration found." -ForegroundColor Red
        exit 1
    }
} else {
    # Save the current configuration for future runs
    Set-Content -Path $lastConfigFile -Value $Config
}

# Set terminal title
$host.UI.RawUI.WindowTitle = "AAASeed - $Action $Config"

# Split multiple configurations by semicolon
$configs = $Config -split ";"
$succeededConfigs = @()
$failedConfigs = @()

foreach ($cfgRaw in $configs) {
    $cfg = $cfgRaw.Trim()
    if (-not $cfg) { continue }

    # Determine toolset based on configuration name
    if ($cfg -like "*_v145") {
        $Toolset = "v145"
    } else {
        $Toolset = "v143"
    }

    Write-Host "=== $Action $cfg (Toolset $Toolset) ===" -ForegroundColor Yellow

    # Run MSBuild
    msbuild "AAASeed_by_Maa.vcxproj" `
        /m:6 `
        /t:$Action `
        /p:Configuration=$cfg `
        /p:Platform=x64 `
        /p:PlatformToolset=$Toolset `
        /p:PlatformShortName=x64 `
        /p:PreferredToolArchitecture=x64 `
        /v:m

    # Check result
    if ($LASTEXITCODE -ne 0) {
        $failedConfigs += $cfg
        Write-Host "$cfg FAILED" -ForegroundColor Red
    } else {
        $succeededConfigs += $cfg
        Write-Host "$cfg succeeded" -ForegroundColor Green
    }
}

# Final summary
Write-Host ""
Write-Host "============================== Build Summary ==============================" -ForegroundColor Yellow

if ($succeededConfigs.Count -gt 0) {
    $succeededList = $succeededConfigs -join ", "
    Write-Host "Succeeded configurations: $succeededList" -ForegroundColor Green
}

if ($failedConfigs.Count -gt 0) {
    $failedList = $failedConfigs -join ", "
    Write-Host "Failed configurations: $failedList" -ForegroundColor Red
    exit 1
} else {
    Write-Host "All configurations succeeded!" -ForegroundColor Green
    exit 0
}
