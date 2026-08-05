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
git checkout releases/v0.39.0

# Export the recipe to the cache so we can skip rebuilds gracefully
conan export .

# Dig out the declared version of the module so we can use it for arguments to --build and --requires below.
$VIAM_CPP_SDK_VERSION = (conan inspect -vquiet . --format=json | ConvertFrom-Json).version

# Build the C++ SDK repo.
#
# We want a static binary, so we turn off shared. Elect for C++17
# compilation, since it seems some of the dependencies we pick mandate
# it anyway. Pin to the Windows 10 1809 associated windows SDK, and
# opt for the static compiler runtime so we don't have a dependency on
# the VC redistributable.
conan install --update `
      --build=missing `
      --requires=viam-cpp-sdk/$VIAM_CPP_SDK_VERSION `
      -s:a build_type=Release `
      -s:a "&:build_type=RelWithDebInfo" `
      -s:a compiler.cppstd=17 `
      -o:a "*:shared=False" `
      -o:a "&:shared=False" `
      -o:a "grpc/*:csharp_plugin=False" `
      -o:a "grpc/*:node_plugin=False" `
      -o:a "grpc/*:objective_c_plugin=False" `
      -o:a "grpc/*:php_plugin=False" `
      -o:a "grpc/*:python_plugin=False" `
      -o:a "grpc/*:ruby_plugin=False" `
      -o:a "grpc/*:otel_plugin=False" `
      -c:a tools.microsoft:winsdk_version=10.0.17763.0 `
      -c:a tools.cmake.cmaketoolchain:extra_variables="{'gRPC_MSVC_STATIC_RUNTIME': 'ON'}" `
      -s:a compiler.runtime=static

# Clean up
Pop-Location  # viam-cpp-sdk
Pop-Location  # tmp_cpp_sdk
Remove-Item -Recurse -Force tmp_cpp_sdk
