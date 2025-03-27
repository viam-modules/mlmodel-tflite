#!/bin/bash
# set -e: exit with errors if anything fails
#     -u: it's an error to use an undefined variable
#     -x: print out every command before it runs
#     -o pipefail: if something in the middle of a pipeline fails, the whole thing fails
set -euxo pipefail

# Set up conan
pip install conan
conan profile detect || echo "Conan is already installed"

# Clone the C++ SDK repo
mkdir -p tmp_cpp_sdk
pushd tmp_cpp_sdk
git clone https://github.com/viamrobotics/viam-cpp-sdk.git
pushd viam-cpp-sdk

# Build the C++ SDK repo
conan install . --output-folder=build-conan --build=missing
cmake . --preset=conan-release
cmake --build --preset=conan-release -j 8
conan create . -o "&:shared=False" --build=missing

popd  # viam-cpp-sdk
popd  # tmp_cpp_sdk
rm -rf tmp_cpp_sdk

# Set up the linter
if ! command -v clang-format &> /dev/null; then
	# It's not yet installed, so let's get it!
	echo "Installing clang-format as a linter..."
	if [[ "$(uname)" == "Linux" ]]; then
		sudo apt install clang-format
	elif [[ "$(uname)" == "Darwin" ]]; then
		brew install clang-format
	else
		echo "WARNING: installing the linter is not yet supported outside of Linux and Mac."
	fi
fi
