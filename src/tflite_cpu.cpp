// Copyright 2025 Viam Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fstream>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>

#include <tensorflow/lite/c/c_api.h>
#include <tensorflow/lite/interpreter_builder.h>
#include <tensorflow/lite/kernels/register.h>

#include <viam/sdk/common/instance.hpp>
#include <viam/sdk/common/proto_value.hpp>
#include <viam/sdk/components/component.hpp>
#include <viam/sdk/config/resource.hpp>
#include <viam/sdk/module/service.hpp>
#include <viam/sdk/registry/registry.hpp>
#include <viam/sdk/resource/reconfigurable.hpp>
#include <viam/sdk/resource/stoppable.hpp>
#include <viam/sdk/rpc/server.hpp>
#include <viam/sdk/services/mlmodel.hpp>

#include "tflite_cpu.hpp"
