:: Delegate to powershell rather than trying to script in batch
Powershell.exe -ExecutionPolicy Bypass -Command "& { bin\package.ps1 %*; exit $LASTEXITCODE }"
