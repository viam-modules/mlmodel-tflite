# Fail fast
$ErrorActionPreference = "Stop"

# Set up conan and other friends we met along the way
choco install -y conan cmake git 7zip

# Ensure that things installed with choco are visible to us
Import-Module $env:ChocolateyInstall\helpers\chocolateyProfile.psm1
refreshenv

# Initialize conan if it hasn't been already
conan profile detect
if (!$?) { Write-Host "Conan is already installed" }

# Clone the C++ SDK repo
mkdir tmp_cpp_sdk
Push-Location tmp_cpp_sdk
git clone https://github.com/viamrobotics/viam-cpp-sdk.git
Push-Location viam-cpp-sdk

# NOTE: If you change this version, also change it in the `conanfile.py` requirements
git checkout releases/v0.13.0

# Build the C++ SDK repo.
#
# We want a static binary, so we turn off shared. Elect for C++17
# compilation, since it seems some of the dependencies we pick mandate
# it anyway. Pin to the Windows 10 1809 associated windows SDK, and
# opt for the static compiler runtime so we don't have a dependency on
# the VC redistributable.
#
# TODO: Note `-tf ""`, which disables the self test. I have not been
# able to get this working on windows.
conan create . `
      --build=missing `
      -o:a "&:shared=False" `
      -s:a build_type=Release `
      -s:a compiler.cppstd=17 `
      -c:a tools.microsoft:winsdk_version=10.0.17763.0 `
      -s:a compiler.runtime=static `
      -tf `"`"

# Clean up
Pop-Location  # viam-cpp-sdk
Pop-Location  # tmp_cpp_sdk
Remove-Item -Recurse -Force tmp_cpp_sdk
