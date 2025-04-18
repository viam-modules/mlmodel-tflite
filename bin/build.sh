#!/bin/bash
# set -e: exit with errors if anything fails
#     -u: it's an error to use an undefined variable
#     -x: print out every command before it runs
#     -o pipefail: if something in the middle of a pipeline fails, the whole thing fails
set -euxo pipefail

# Clean up any prior build
rm -rf build-conan

# Build the tflite_cpu module
#
# We want a static binary, so we turn off shared. Elect for C++17
# compilation, since it seems some of the dependencies we pick mandate
# it anyway.
conan build . \
      --output-folder=build-conan \
      --build=missing \
      -o "&:shared=False" \
      -s:h compiler.cppstd=17
