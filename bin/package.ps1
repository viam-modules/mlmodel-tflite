# Fail fast
$ErrorActionPreference = "Stop"

# Ensure that things installed with choco are visible to us
Import-Module $env:ChocolateyInstall\helpers\chocolateyProfile.psm1
refreshenv

# Fixup `meta.json` to have `.exe at the end
Copy-Item meta.json meta.json.backup
jq -f .\bin\fixup_windows_entrypoint.jq meta.json.backup | Out-File -FilePath meta.json -Encoding ASCII

# Tar up all the command line arguments, then gzip them
7z a -ttar module.tar $args
7z a -tgzip -sdel module.tar.gz module.tar

# Clean house. Keep the updated meta.json, which is needed for uploading the module from Github.
if (Test-Path "meta.windows.json") {
    Remove-Item "meta.windows.json"
}
Move-Item meta.json meta.windows.json
Move-Item meta.json.backup meta.json
