#!/bin/bash
# set -e: exit with errors if anything fails
#     -u: it's an error to use an undefined variable
#     -x: print out every command before it runs
#     -o pipefail: if something in the middle of a pipeline fails, the whole thing fails
set -euxo pipefail

rm -rf build-conan

conan build . \
      --output-folder=build-conan \
      --build=missing \
      -o "&:shared=False" \
      -s:h compiler.cppstd=17
