# Fail fast
$ErrorActionPreference = "Stop"

# Ensure that things installed with choco are visible to us
Import-Module $env:ChocolateyInstall\helpers\chocolateyProfile.psm1
refreshenv

# Tar up all the command line arguments, then gzip them
7z a -ttar module.tar $args
7z a -tgzip -sdel module.tar.gz module.tar
