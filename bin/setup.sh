#!/bin/bash
# set -e: exit with errors if anything fails
#     -u: it's an error to use an undefined variable
#     -x: print out every command before it runs
#     -o pipefail: if something in the middle of a pipeline fails, the whole thing fails
set -euxo pipefail

# Set up conan
conan --version > /dev/null 2>&1 || python -m pip install conan
conan profile detect || echo "Conan is already installed"

# Clone the C++ SDK repo
mkdir -p tmp_cpp_sdk
pushd tmp_cpp_sdk
git clone https://github.com/viamrobotics/viam-cpp-sdk.git
pushd viam-cpp-sdk

# NOTE: If you change this version, also change it in the `conanfile.py` requirements
git checkout releases/v0.39.0

# Export the recipe to the cache so we can skip rebuilds gracefully
conan export .

# Dig out the declared version of the module so we can use it for arguments to --build and --requires below.
VIAM_CPP_SDK_VERSION=$(conan inspect -vquiet . --format=json | jq -r '.version')

# Build the C++ SDK repo
#
# We want a static binary, so we turn off shared. Elect for C++17
# compilation, since it seems some of the dependencies we pick mandate
# it anyway.

conan install --update \
      --build=missing \
      --requires=viam-cpp-sdk/${VIAM_CPP_SDK_VERSION} \
      -s:a build_type=Release \
      -s:a "&:build_type=RelWithDebInfo" \
      -s:a compiler.cppstd=17 \
      -o:a "*:shared=False" \
      -o:a "&:shared=False" \
      -o:a "grpc/*:csharp_plugin=False" \
      -o:a "grpc/*:node_plugin=False" \
      -o:a "grpc/*:objective_c_plugin=False" \
      -o:a "grpc/*:php_plugin=False" \
      -o:a "grpc/*:python_plugin=False" \
      -o:a "grpc/*:ruby_plugin=False" \
      -o:a "grpc/*:otel_plugin=False"

# Cleanup
popd  # viam-cpp-sdk
popd  # tmp_cpp_sdk
rm -rf tmp_cpp_sdk
