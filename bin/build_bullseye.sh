#!/bin/bash
# set -e: exit with errors if anything fails
#     -u: it's an error to use an undefined variable
#     -x: print out every command before it runs
#     -o pipefail: if something in the middle of a pipeline fails, the whole thing fails
set -euxo pipefail

# Raspberry Pi 4 runs Debian Bullseye, which is old enough that it doesn't have a recent enough
# glibc version for code compiled on the Github action runner's oldest version of Ubuntu. So,
# instead, we compile everything in a Debian Bullseye docker container, to ensure it works on such
# an old system. This bash script is what to run in the docker container.

# We assume that the repository is mounted in /repo.
pushd /repo

apt update
apt upgrade -y

# Debian Bullseye is old enough that the default python interpreter is Python2, but we're using
# Python3 features. So, install python3, and then make a copy of it called just python. Don't care
# if we mess up /bin: the entire container is going to be thrown out anyway.
apt install -y cmake curl git libstdc++6 make python3 python3-pip
cp $(which python3) /bin/python

# Boost is installed via b2, but the pre-compiled versions of b2 require a more recent version of
# glibcxx than is available on Debian Bullseye. So, instead, build it ourselves. and that means
# installing conan before we get to the setup script.
python -m pip install conan
conan install -r conancenter --update --tool-requires=b2/5.3.1 --build=b2/5.3.1 -s compiler.cppstd=14 -s:a compiler.cppstd=14

# Finally, build everything!
make setup
make module.tar.gz

popd # /repo
