#!/bin/bash

# Copyright 2025 Viam Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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

find ./src -not -path "./src/viam/api" -type f \( -name \*.cpp -o -name \*.hpp \) | xargs clang-format -style=file -i -fallback-style=none "$@"
