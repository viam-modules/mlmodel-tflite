# Copyright 2023 Viam Inc.
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

cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
set(CMAKE_PROJECT_VERSION 0.0.1)

project(tflite_cpu
    DESCRIPTION "Viam module for tflite inference  on the CPU"
    HOMEPAGE_URL https://github.com/viam-modules/mlmodel-tflite
    LANGUAGES CXX 
)
set(CMAKE_CXX_STANDARD 14)
set(BUILD_SHARED_LIBS OFF)

find_package(Threads REQUIRED)
find_package(viam-cpp-sdk REQUIRED)
find_package(tensorflowlite REQUIRED)

add_executable(tflite_cpu
  src/tflite_cpu.cpp
)

target_link_libraries(tflite_cpu
  PRIVATE Threads::Threads
  PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,viam-cpp-sdk::viamsdk>"
  PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,tensorflow::tensorflowlite>"
)

install(
  TARGETS tflite_cpu
)
