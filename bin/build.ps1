# Fail fast
$ErrorActionPreference = "Stop"

# Ensure that things installed with choco are visible to us
Import-Module $env:ChocolateyInstall\helpers\chocolateyProfile.psm1
refreshenv

# Clean up any prior build
Remove-Item -Recurse -Force build-conan -ErrorAction SilentlyContinue

# Build the tflite_cpu module
#
# We want a static binary, so we turn off shared. Elect for C++17
# compilation, since it seems some of the dependencies we pick mandate
# it anyway. Pin to the Windows 10 1809 associated windows SDK, and
# opt for the static compiler runtime so we don't have a dependency on
# the VC redistributable.
conan build . `
      --output-folder=build-conan `
      --build=missing `
      -o "&:shared=False" `
      -s:h compiler.cppstd=17 `
      -c:h tools.microsoft:winsdk_version=10.0.17763.0 `
      -s:h compiler.runtime=static
