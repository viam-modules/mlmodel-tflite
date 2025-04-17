$ErrorActionPreference = "Stop"

Import-Module $env:ChocolateyInstall\helpers\chocolateyProfile.psm1
refreshenv

# Fixup `meta.json` to have `.exe at the end
Copy-Item meta.json meta.json.backup

jq -f .\bin\fixup_windows_entrypoint.jq meta.json.backup | Out-File -FilePath meta.json -Encoding utf8

7z a -ttar module.tar $args
7z a -tgzip -sdel module.tar.gz module.tar

Remove-Item meta.json
Move-Item meta.json.backup meta.json
