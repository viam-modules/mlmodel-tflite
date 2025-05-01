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

# We could just call `build` here with `--build=missing`, but we split this into two steps
# so we can ensure that all our dependencies are built consistently in `Release`, but that
# the actual module build gets built with an override to `RelWithDebInfo`, which we
# don't want to have accidentally affect our dependencies (it makes the build far too large).
# The override itself is derived from https://github.com/conan-io/conan/issues/12656.

conan install . `
      --build=missing `
      -o:a "&:shared=False" `
      -s:a build_type=Release `
      -s:a compiler.cppstd=17 `
      -c:a tools.microsoft:winsdk_version=10.0.17763.0 `
      -s:a compiler.runtime=static

conan build . `
      --output-folder=build-conan `
      --build=none `
      -o:a "&:shared=False" `
      -s:a build_type=Release `
      -s:a "&:build_type=RelWithDebInfo" `
      -s:a compiler.cppstd=17 `
      -c:a tools.microsoft:winsdk_version=10.0.17763.0 `
      -s:a compiler.runtime=static
