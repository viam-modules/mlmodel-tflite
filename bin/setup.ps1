$ErrorActionPreference = "Stop"

# Set up conan
choco install -y conan cmake git 7zip

Import-Module $env:ChocolateyInstall\helpers\chocolateyProfile.psm1
refreshenv

conan profile detect
if (!$?) { Write-Host "Conan is already installed" }

# Clone the C++ SDK repo
mkdir tmp_cpp_sdk
Push-Location tmp_cpp_sdk
git clone https://github.com/acmorrow/viam-cpp-sdk.git
Push-Location viam-cpp-sdk

# NOTE: If you change this version, also change it in the `conanfile.py` requirements
git checkout releases/v0.9.0

# Build the C++ SDK repo.
#
# TODO: Note `-tf ""`, which disables the self test. I have not been
# able to get this working on windows.
conan create . `
      --build=missing `
      -o "&:shared=False" `
      -s:h compiler.cppstd=17 `
      -c:h tools.microsoft:winsdk_version=10.0.17763.0 `
      -s:h compiler.runtime=static `
      -tf `"`"

Pop-Location  # viam-cpp-sdk
Pop-Location  # tmp_cpp_sdk
Remove-Item -Recurse -Force tmp_cpp_sdk
