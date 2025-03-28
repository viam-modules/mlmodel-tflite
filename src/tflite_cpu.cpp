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
#include <pthread.h>
#include <sstream>
#include <stdexcept>

#include <tensorflow/lite/c/c_api.h>

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

namespace {

namespace vsdk = ::viam::sdk;
constexpr char service_name[] = "viam_tflite_cpu";

// An MLModelService instance which runs TensorFlow Lite models.
//
// Configuration requires the following parameters:
//   -- `model_path`: An absolute filesystem path to a TensorFlow Lite model file.
//
// The following optional parameters are honored:
//   -- `num_threads`: Sets the number of threads to be used, where applicable.
//
//   -- `label_path`:  An absolute filesystem path to a .txt file of the model's category labels.
//
// Any additional configuration fields are ignored.
class MLModelServiceTFLite : public vsdk::MLModelService,
                             public vsdk::Stoppable,
                             public vsdk::Reconfigurable {
    class write_to_tflite_tensor_visitor_;

   public:
    explicit MLModelServiceTFLite(vsdk::Dependencies dependencies,
                                  vsdk::ResourceConfig configuration)
        : MLModelService(configuration.name()),
          state_(configure_(std::move(dependencies), std::move(configuration))) {}

    ~MLModelServiceTFLite() final {
        // All invocations arrive via gRPC, so we know we are idle
        // here. It should be safe to tear down all state
        // automatically without needing to wait for anything more to
        // drain.
    }

    void stop(const vsdk::ProtoStruct& extra) noexcept final {
        return stop();
    }

    /// @brief Stops the MLModelServiceTFLite from running.
    void stop() noexcept {
        const std::lock_guard<std::mutex> lock(state_lock_);
        state_.reset();
    }

    void reconfigure(const vsdk::Dependencies& dependencies,
                     const vsdk::ResourceConfig& configuration) final {


        const std::lock_guard<std::mutex> lock(state_lock_);
        check_stopped_inlock_();
        state_.reset();
        state_ = configure_(dependencies, configuration);
    }

    std::shared_ptr<named_tensor_views> infer(const named_tensor_views& inputs,
                                              const vsdk::ProtoStruct& extra) final {

        // We serialize access to the interpreter. We use a
        // `unique_lock` instead of a `lock_guard` because we will
        // move the lock into the shared state that we return,
        // allowing the higher level to effect a direct copy out of
        // the tflite buffers while the interpreter is still locked.
        std::unique_lock<std::mutex> lock(state_lock_);
        check_stopped_inlock_();

        // Special case: if the model has only one input tensor and only one tensor is provided,
        // use it regardless of name
        if (state_->input_tensor_indices_by_name.size() == 1 && inputs.size() == 1) {
            const auto& input_pair = *inputs.begin();
            const auto& tensor_name = input_pair.first;
            const auto& tensor_view = input_pair.second;

            // Get the first (and only) input tensor index
            int tensor_index = state_->input_tensor_indices_by_name.begin()->second;

            auto* const tensor = TfLiteInterpreterGetInputTensor(state_->interpreter.get(), tensor_index);
            if (!tensor) {
                std::ostringstream buffer;
                buffer << service_name
                       << ": Failed to obtain tflite input tensor for `" << tensor_name << "`, the only input";
                throw std::invalid_argument(buffer.str());
            }

            const auto tflite_status =
                boost::apply_visitor(write_to_tflite_tensor_visitor_(&tensor_name, tensor), tensor_view);

            if (tflite_status != TfLiteStatus::kTfLiteOk) {
                std::ostringstream buffer;
                buffer << service_name
                       << ": input tensor `" << tensor_name << "` failed population: "
                       << state_->interpreter_error_data;
                throw std::invalid_argument(buffer.str());
            }
        } else {
            // Ensure that enough inputs were provided.
            if (inputs.size() < state_->input_tensor_indices_by_name.size()) {
                std::ostringstream buffer;
                buffer << service_name
                       << ": Too few inputs provided for inference: " << state_->input_tensor_indices_by_name.size()
                       << " expected, but got " << inputs.size() << " instead";
                throw std::invalid_argument(buffer.str());
            }
            // Walk the inputs, and copy the data from each of the input
            // tensor views we were given into the associated tflite input
            // tensor buffer.
            for (const auto& kv : inputs) {
                const auto where = state_->input_tensor_indices_by_name.find(kv.first);
                if (where == state_->input_tensor_indices_by_name.end()) {
                    std::ostringstream buffer;
                    buffer << service_name << ": Tensor name `" << kv.first << "`"
                           << " is not a known input tensor name for the model";
                    throw std::invalid_argument(buffer.str());
                }
                auto* const tensor =
                    TfLiteInterpreterGetInputTensor(state_->interpreter.get(), where->second);
                if (!tensor) {
                    std::ostringstream buffer;
                    buffer << service_name << ": Failed to obtain tflite input tensor for `" << kv.first
                           << "` (index " << where->second << ")";
                    throw std::invalid_argument(buffer.str());
                }

                const auto tflite_status =
                    boost::apply_visitor(write_to_tflite_tensor_visitor_(&kv.first, tensor), kv.second);

                if (tflite_status != TfLiteStatus::kTfLiteOk) {
                    std::ostringstream buffer;
                    buffer << service_name
                           << ": input tensor `" << kv.first
                           << "` failed population: " << state_->interpreter_error_data;
                    throw std::invalid_argument(buffer.str());
                }
            }
        }

        // Invoke the interpreter and return any failure information.
        const auto tflite_status = TfLiteInterpreterInvoke(state_->interpreter.get());
        if (tflite_status != TfLiteStatus::kTfLiteOk) {
            std::ostringstream buffer;
            buffer << service_name
                   << ": interpreter invocation failed: " << state_->interpreter_error_data;
            throw std::runtime_error(buffer.str());
        }

        // A local type that we will keep on the heap to hold
        // inference results until the caller is done with them. In
        // our case, the caller is MLModelServiceServer, which will
        // copy the data into the reply gRPC proto and then unwind. So
        // we can avoid copying the data by letting the views alias
        // the tensorflow tensor buffers and keep the interpreter lock
        // held until the gRPC work is done. Note that this means the
        // interpreter lock will remain held until the
        // inference_result_type object tracked by the shared pointer
        // we return is destroyed. Callers that want to make use of
        // the inference results without keeping the interpreter
        // locked would need to copy the data out of the views and
        // then release the return value.
        struct inference_result_type {
            std::unique_lock<std::mutex> state_lock;
            named_tensor_views views;
        };
        auto inference_result = std::make_shared<inference_result_type>();

        // Walk the outputs per our metadata and emplace an
        // appropriately typed tensor_view aliasing the interpreter
        // output tensor buffer into the inference results.
        for (const auto& output : state_->metadata.outputs) {
            const auto where = state_->output_tensor_indices_by_name.find(output.name);
            if (where == state_->output_tensor_indices_by_name.end()) {
                continue;  // Should be impossible
            }
            const auto* const tflite_tensor =
                TfLiteInterpreterGetOutputTensor(state_->interpreter.get(), where->second);
            inference_result->views.emplace(output.name,
                                            std::move(make_tensor_view_(output, tflite_tensor)));
        }

        // The views created in the loop above are only valid until
        // the interpreter lock is released, so we keep the lock held
        // by moving the unique_lock into the inference_result
        // object.
        inference_result->state_lock = std::move(lock);

        // Finally, construct an aliasing shared_ptr which appears to
        // the caller as a shared_ptr to views, but in fact manages
        // the lifetime of the inference_result. When the
        // inference_result object is destroyed, the lock will be
        // released and the next caller can invoke the interpreter.
        auto* const views = &inference_result->views;
        // NOLINTNEXTLINE(performance-move-const-arg): C++20
        return {std::move(inference_result), views};
    }

    struct metadata metadata(const vsdk::ProtoStruct& extra) final {
        // Just return a copy of our metadata from leased state.
        std::lock_guard<std::mutex> lock(state_lock_);
        check_stopped_inlock_();
        return state_->metadata;
    }

   private:
    struct state_;

    void check_stopped_inlock_() const {
        if (!state_) {
            std::ostringstream buffer;
            buffer << service_name << ": service is stopped: ";
            throw std::runtime_error(buffer.str());
        }
    }

    static std::unique_ptr<struct state_> configure_(vsdk::Dependencies dependencies,
                                                     vsdk::ResourceConfig configuration) {

        auto state =
            std::make_unique<struct state_>(std::move(dependencies), std::move(configuration));

        // Now we can begin parsing and validating the provided `configuration`.
        // Pull the model path out of the configuration.
        const auto& attributes = state->configuration.attributes();
        auto model_path = attributes.find("model_path");
        if (model_path == attributes.end()) {
            std::ostringstream buffer;
            buffer << service_name
                   << ": Required parameter `model_path` not found in configuration";
            throw std::invalid_argument(buffer.str());
        }
        const auto* const model_path_string = model_path->second.get<std::string>();
        if (!model_path_string || model_path_string->empty()) {
            std::ostringstream buffer;
            buffer << service_name
                   << ": Required non-empty string parameter `model_path` is either not a string "
                      "or is an empty string";
            throw std::invalid_argument(buffer.str());
        }

        std::string label_path_string = ""; // default value for label_path
        auto label_path = attributes.find("label_path");
        if (label_path != attributes.end()) {
            const auto* const lp_string = label_path->second.get<std::string>();
            if (!lp_string) {
                std::ostringstream buffer;
                buffer << service_name
                       << ": string parameter `label_path` is not a string ";
                throw std::invalid_argument(buffer.str());
            }
            label_path_string = *lp_string;
        }
        state->label_path = std::move(label_path_string);

        // Configuration parsing / extraction is complete. Move on to
        // building the actual model with the provided information.

        // Try to load the provided `model_path`. The TFLite API
        // declares that if you use `TfLiteModelCreateFromFile` that
        // the file must remain unaltered during execution, but
        // reconfiguration might cause it to change on disk while
        // inference is in progress. Instead we read the file into a
        // buffer which we can use with `TfLiteModelCreate`. That
        // still requires that the buffer be kept valid, but that's
        // more easily done.
        const std::ifstream in(*model_path_string, std::ios::in | std::ios::binary);
        if (!in) {
            std::ostringstream buffer;
            buffer << service_name << ": Failed to open file for `model_path` "
                   << *model_path_string;
            throw std::invalid_argument(buffer.str());
        }
        std::ostringstream model_path_contents_stream;
        model_path_contents_stream << in.rdbuf();
        state->model_data = std::move(model_path_contents_stream.str());

        // Create an error reporter so that we can extract detailed
        // error information from TFLite when things go wrong. The
        // error state is protected by the interpreter lock.
        state->model.reset(TfLiteModelCreateWithErrorReporter(
            state->model_data.data(),
            state->model_data.size(),
            [](void* ud, const char* fmt, va_list args) {
                char buffer[4096];
                static_cast<void>(vsnprintf(buffer, sizeof(buffer), fmt, args));
                *reinterpret_cast<std::string*>(ud) = buffer;
            },
            &state->interpreter_error_data));

        // If we failed to create the model, return an error and
        // include the error data that tflite wrote to the error
        // reporter state.
        if (!state->model) {
            std::ostringstream buffer;
            buffer << service_name << ": Failed to load model from file `" << model_path_string
                   << "`: " << state->interpreter_error_data;
            throw std::invalid_argument(buffer.str());
        }

        // If present, extract and validate the number of threads to
        // use in the interpreter and create an interpreter options
        // object to carry that information.
        auto num_threads = attributes.find("num_threads");
        if (num_threads != attributes.end()) {
            const auto* num_threads_double = num_threads->second.get<double>();
            if (!num_threads_double || !std::isnormal(*num_threads_double) ||
                (*num_threads_double < 0) ||
                (*num_threads_double >= std::numeric_limits<std::int32_t>::max()) ||
                (std::trunc(*num_threads_double) != *num_threads_double)) {
                std::ostringstream buffer;
                buffer << service_name
                       << ": Value for field `num_threads` is not a positive integer: "
                       << *num_threads_double;
                throw std::invalid_argument(buffer.str());
            }

            state->interpreter_options.reset(TfLiteInterpreterOptionsCreate());
            TfLiteInterpreterOptionsSetNumThreads(state->interpreter_options.get(),
                                                  static_cast<int32_t>(*num_threads_double));
        }

        // Build the single interpreter.
        state->interpreter.reset(
            TfLiteInterpreterCreate(state->model.get(), state->interpreter_options.get()));
        if (!state->interpreter) {
            std::ostringstream buffer;
            buffer << service_name
                   << ": Failed to create tflite interpreter: " << state->interpreter_error_data;
            throw std::runtime_error(buffer.str());
        }

        // Have the interpreter allocate tensors for the model
        auto tfresult = TfLiteInterpreterAllocateTensors(state->interpreter.get());
        if (tfresult != kTfLiteOk) {
            std::ostringstream buffer;
            buffer << service_name << ": Failed to allocate tensors for tflite interpreter: "
                   << state->interpreter_error_data;
            throw std::runtime_error(buffer.str());
        }

        // Walk the input tensors now that they have been allocated
        // and extract information about tensor names, types, and
        // dimensions. Apply any tensor renamings per our
        // configuration. Stash the relevant data in our `metadata`
        // fields.
        auto num_input_tensors = TfLiteInterpreterGetInputTensorCount(state->interpreter.get());
        for (decltype(num_input_tensors) i = 0; i != num_input_tensors; ++i) {
            const auto* const tensor = TfLiteInterpreterGetInputTensor(state->interpreter.get(), i);

            auto ndims = TfLiteTensorNumDims(tensor);
            if (ndims == -1) {
                std::ostringstream buffer;
                buffer << service_name
                       << ": Unable to determine input tensor shape at configuration time, "
                          "inference not possible";
                throw std::runtime_error(buffer.str());
            }

            MLModelService::tensor_info input_info;
            const auto* name = TfLiteTensorName(tensor);
            input_info.name = name;
            input_info.data_type =
                service_data_type_from_tflite_data_type_(TfLiteTensorType(tensor));
            for (decltype(ndims) j = 0; j != ndims; ++j) {
                input_info.shape.push_back(TfLiteTensorDim(tensor, j));
            }
            state->input_tensor_indices_by_name[input_info.name] = i;
            state->metadata.inputs.emplace_back(std::move(input_info));
        }

        // As above, but for output tensors.
        //
        // NOTE: The tflite C API docs state that information about
        // output tensors may not be available until after one round
        // of inference. We are ignoring that guidance for now per the
        // unknowns about how metadata will be handled in the future.
        auto num_output_tensors = TfLiteInterpreterGetOutputTensorCount(state->interpreter.get());
        const auto* const output_tensor_ixes =
            TfLiteInterpreterOutputTensorIndices(state->interpreter.get());
        for (decltype(num_output_tensors) i = 0; i != num_output_tensors; ++i) {
            const auto* const tensor =
                TfLiteInterpreterGetOutputTensor(state->interpreter.get(), i);

            auto ndims = TfLiteTensorNumDims(tensor);
            if (ndims == -1) {
                std::ostringstream buffer;
                buffer << service_name
                       << ": Unable to determine output tensor shape at configuration time, "
                          "inference not possible";
                throw std::runtime_error(buffer.str());
            }

            MLModelService::tensor_info output_info;
            const auto* name = TfLiteTensorName(tensor);
            output_info.name = name;
            output_info.data_type =
                service_data_type_from_tflite_data_type_(TfLiteTensorType(tensor));
            for (decltype(ndims) j = 0; j != ndims; ++j) {
                output_info.shape.push_back(TfLiteTensorDim(tensor, j));
            }
            if (state->label_path != "") {
                output_info.extra.insert({"labels", state->label_path});
            }
            state->output_tensor_indices_by_name[output_info.name] = i;
            state->metadata.outputs.emplace_back(std::move(output_info));
        }

        return state;
    }

    // Converts from tflites type enumeration into the model service
    // type enumeration or throws if there is no such conversion.
    static MLModelService::tensor_info::data_types service_data_type_from_tflite_data_type_(
        TfLiteType type) {
        switch (type) {
            case kTfLiteInt8: {
                return MLModelService::tensor_info::data_types::k_int8;
            }
            case kTfLiteUInt8: {
                return MLModelService::tensor_info::data_types::k_uint8;
            }
            case kTfLiteInt16: {
                return MLModelService::tensor_info::data_types::k_int16;
            }
            case kTfLiteUInt16: {
                return MLModelService::tensor_info::data_types::k_uint16;
            }
            case kTfLiteInt32: {
                return MLModelService::tensor_info::data_types::k_int32;
            }
            case kTfLiteUInt32: {
                return MLModelService::tensor_info::data_types::k_uint32;
            }
            case kTfLiteInt64: {
                return MLModelService::tensor_info::data_types::k_int64;
            }
            case kTfLiteUInt64: {
                return MLModelService::tensor_info::data_types::k_uint64;
            }
            case kTfLiteFloat32: {
                return MLModelService::tensor_info::data_types::k_float32;
            }
            case kTfLiteFloat64: {
                return MLModelService::tensor_info::data_types::k_float64;
            }
            default: {
                std::ostringstream buffer;
                buffer << service_name << ": Model contains unsupported tflite data type" << type;
                throw std::invalid_argument(buffer.str());
            }
        }
    }

    // All of the meaningful internal state of the service is held in
    // a separate state object to help ensure clean replacement of our
    // internals during reconfiguration.
    struct state_ {
        explicit state_(vsdk::Dependencies dependencies, vsdk::ResourceConfig configuration)
            : dependencies(std::move(dependencies)), configuration(std::move(configuration)) {}

        // The dependencies and configuration we were given at
        // construction / reconfiguration.
        vsdk::Dependencies dependencies;
        vsdk::ResourceConfig configuration;

        // This data must outlive any interpreters created from the
        // model we build against model data.
        std::string model_data;

        // Technically, we don't need to keep the model after we create an interpreter,
        // but it may be useful to do so in case we ever want to pool interpreters.
        std::unique_ptr<TfLiteModel, decltype(&TfLiteModelDelete)> model{nullptr,
                                                                         &TfLiteModelDelete};

        // Similarly, keep the options we built around for potential
        // re-use.
        std::unique_ptr<TfLiteInterpreterOptions, decltype(&TfLiteInterpreterOptionsDelete)>
            interpreter_options{nullptr, &TfLiteInterpreterOptionsDelete};

        // Metadata about input and output tensors that was extracted
        // during configuration. Callers need this in order to know
        // how to interact with the service.
        struct MLModelService::metadata metadata;

        //  The label path is a file that relates the outputs of the label tensor ints to strings
        std::string label_path;

        // Maps from string names of tensors to the numeric
        // value. Note that the keys here are the renamed tensors, if
        // applicable.
        std::unordered_map<std::string, int> input_tensor_indices_by_name;
        std::unordered_map<std::string, int> output_tensor_indices_by_name;

        // The configured error reporter will overwrite this string.
        std::string interpreter_error_data;

        // The interpreter itself.
        std::unique_ptr<TfLiteInterpreter, decltype(&TfLiteInterpreterDelete)> interpreter{
            nullptr, &TfLiteInterpreterDelete};
    };

    // A visitor that can populate a TFLiteTensor given a MLModelService::tensor_view.
    class write_to_tflite_tensor_visitor_ : public boost::static_visitor<TfLiteStatus> {
       public:
        write_to_tflite_tensor_visitor_(const std::string* name, TfLiteTensor* tflite_tensor)
            : name_(name), tflite_tensor_(tflite_tensor) {}

        template <typename T>
        TfLiteStatus operator()(const T& mlmodel_tensor) const {
            const auto expected_size = TfLiteTensorByteSize(tflite_tensor_);
            const auto* const mlmodel_data_begin =
                reinterpret_cast<const unsigned char*>(mlmodel_tensor.data());
            const auto* const mlmodel_data_end = reinterpret_cast<const unsigned char*>(
                mlmodel_tensor.data() + mlmodel_tensor.size());
            const auto mlmodel_data_size =
                static_cast<size_t>(mlmodel_data_end - mlmodel_data_begin);
            if (expected_size != mlmodel_data_size) {
                std::ostringstream buffer;
                buffer << service_name << ": tensor `" << *name_
                       << "` was expected to have byte size " << expected_size << " but "
                       << mlmodel_data_size << " bytes were provided";
                throw std::invalid_argument(buffer.str());
            }
            return TfLiteTensorCopyFromBuffer(tflite_tensor_, mlmodel_data_begin, expected_size);
        }

       private:
        const std::string* name_;
        TfLiteTensor* tflite_tensor_;
    };

    // Creates a tensor_view which views a tflite tensor buffer. It dispatches on the
    // type and delegates to the templated version below.
    MLModelService::tensor_views make_tensor_view_(const MLModelService::tensor_info& info,
                                                   const TfLiteTensor* const tflite_tensor) {
        const auto tflite_tensor_type = TfLiteTensorType(tflite_tensor);
        switch (tflite_tensor_type) {
            case kTfLiteInt8: {
                return make_tensor_view_t_<std::int8_t>(info, tflite_tensor);
            }
            case kTfLiteUInt8: {
                return make_tensor_view_t_<std::uint8_t>(info, tflite_tensor);
            }
            case kTfLiteInt16: {
                return make_tensor_view_t_<std::int16_t>(info, tflite_tensor);
            }
            case kTfLiteUInt16: {
                return make_tensor_view_t_<std::uint16_t>(info, tflite_tensor);
            }
            case kTfLiteInt32: {
                return make_tensor_view_t_<std::int32_t>(info, tflite_tensor);
            }
            case kTfLiteUInt32: {
                return make_tensor_view_t_<std::uint32_t>(info, tflite_tensor);
            }
            case kTfLiteInt64: {
                return make_tensor_view_t_<std::int64_t>(info, tflite_tensor);
            }
            case kTfLiteUInt64: {
                return make_tensor_view_t_<std::uint64_t>(info, tflite_tensor);
            }
            case kTfLiteFloat32: {
                return make_tensor_view_t_<float>(info, tflite_tensor);
            }
            case kTfLiteFloat64: {
                return make_tensor_view_t_<double>(info, tflite_tensor);
            }
            default: {
                std::ostringstream buffer;
                buffer << service_name
                       << ": Model returned unsupported tflite data type: " << tflite_tensor_type;
                throw std::invalid_argument(buffer.str());
            }
        }
    }

    // The type specific version of the above function, it just
    // reinterpret_casts the tensor buffer into an MLModelService
    // tensor view and applies the necessary shape info.
    template <typename T>
    MLModelService::tensor_views make_tensor_view_t_(const MLModelService::tensor_info& info,
                                                     const TfLiteTensor* const tflite_tensor) {
        const auto* const tensor_data = reinterpret_cast<const T*>(TfLiteTensorData(tflite_tensor));
        const auto tensor_size_bytes = TfLiteTensorByteSize(tflite_tensor);
        const auto tensor_size_t = tensor_size_bytes / sizeof(T);
        // TODO: We are just feeding back out what we cached in the
        // metadata for shape. Should this instead be re-querying the
        // output tensor NumDims / DimN after each invocation in case
        // the shape is dynamic? The possibility of a dynamically
        // sized extent is why we represent the dimensions as signed
        // quantities in the tensor metadata. But an actual tensor has
        // a real extent. How would tflite ever communicate that to us
        // differently given that we use the same API to obtain
        // metadata as we would here?
        std::vector<std::size_t> shape;
        shape.reserve(info.shape.size());
        for (const auto s : info.shape) {
            shape.push_back(static_cast<std::size_t>(s));
        }
        return MLModelService::make_tensor_view(tensor_data, tensor_size_t, std::move(shape));
    }

    // Accesss to the module state is serialized. All configuration
    // state is held in the `state` type to make it easier to destroy
    // the current state and replace it with a new one.
    std::mutex state_lock_;

    // In C++17, this could be `std::optional`.
    std::unique_ptr<struct state_> state_;
};

int serve(const std::string& socket_path) try {
    // Every Viam C++ SDK program must have one and only one Instance object which is created before
    // any other C++ SDK objects and stays alive until all Viam C++ SDK objects are destroyed.
    vsdk::Instance inst;

    // Create a new model registration for the service.
    auto module_registration = std::make_shared<vsdk::ModelRegistration>(
        // Identify that this resource offers the MLModelService API
        vsdk::API::get<vsdk::MLModelService>(),

        // Declare a model triple for this service.
        vsdk::Model{"viam", "mlmodel-tflite", "tflite_cpu"},

        // Define the factory for instances of the resource.
        [](vsdk::Dependencies deps, vsdk::ResourceConfig config) {
            return std::make_shared<MLModelServiceTFLite>(std::move(deps), std::move(config));
        });

    // Register the newly created registration with the Registry.
    vsdk::Registry::get().register_model(module_registration);

    // Construct the module service and tell it where to place the socket path.
    auto module_service = std::make_shared<vsdk::ModuleService>(socket_path);

    // Add the server as providing the API and model declared in the
    // registration.
    module_service->add_model_from_registry(module_registration->api(),
                                            module_registration->model());

    // Start the module service.
    module_service->serve();

    return EXIT_SUCCESS;
} catch (const std::exception& ex) {
    std::cout << "ERROR: A std::exception was thrown from `serve`: " << ex.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cout << "ERROR: An unknown exception was thrown from `serve`" << std::endl;
    return EXIT_FAILURE;
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string usage = "usage: mlmodelservice_tflite /path/to/unix/socket";

    if (argc < 2) {
        std::cout << "ERROR: insufficient arguments\n";
        std::cout << usage << "\n";
        return EXIT_FAILURE;
    }

    return serve(argv[1]);
}
