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

namespace mlmodel_tflite
{

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
    class MLModelServiceTFLite : public mlmodel_tflite::vsdk::MLModelService,
                                 public mlmodel_tflite::vsdk::Stoppable,
                                 public mlmodel_tflite::vsdk::Reconfigurable
    {
        class write_to_tflite_tensor_visitor_;

    public:
        explicit MLModelServiceTFLite(mlmodel_tflite::vsdk::Dependencies dependencies,
                                      mlmodel_tflite::vsdk::ResourceConfig configuration)
            : MLModelService(configuration.name()),
              state_(configure_(std::move(dependencies), std::move(configuration))) {}

        ~MLModelServiceTFLite() final;

        void stop(const mlmodel_tflite::vsdk::ProtoStruct &extra) noexcept final;

        /// @brief Stops the MLModelServiceTFLite from running.
        void stop() noexcept;

        void reconfigure(const mlmodel_tflite::vsdk::Dependencies &dependencies,
                         const mlmodel_tflite::vsdk::ResourceConfig &configuration) final;

        std::shared_ptr<named_tensor_views> infer(const named_tensor_views &inputs,
                                                  const mlmodel_tflite::vsdk::ProtoStruct &extra) final;

    private:
        struct state_;

        void check_stopped_inlock_() const;

        static std::unique_ptr<struct state_> configure_(mlmodel_tflite::vsdk::Dependencies dependencies,
                                                         mlmodel_tflite::vsdk::ResourceConfig configuration);

        static MLModelService::tensor_info::data_types service_data_type_from_tflite_data_type_(
            TfLiteType type);

        // All of the meaningful internal state of the service is held in
        // a separate state object to help ensure clean replacement of our
        // internals during reconfiguration.
        // struct state_ final : public tflite::ErrorReporter;

        // A visitor that can populate a TFLiteTensor given a MLModelService::tensor_view.
        class write_to_tflite_tensor_visitor_ : public boost::static_visitor<TfLiteStatus>
        {
        public:
            write_to_tflite_tensor_visitor_(const std::string *name, TfLiteTensor *tflite_tensor)
                : name_(name), tflite_tensor_(tflite_tensor) {};

            template <typename T>
            TfLiteStatus operator()(const T &mlmodel_tensor) const
            {
                const auto expected_size = TfLiteTensorByteSize(tflite_tensor_);
                const auto *const mlmodel_data_begin =
                    reinterpret_cast<const unsigned char *>(mlmodel_tensor.data());
                const auto *const mlmodel_data_end = reinterpret_cast<const unsigned char *>(
                    mlmodel_tensor.data() + mlmodel_tensor.size());
                const auto mlmodel_data_size =
                    static_cast<size_t>(mlmodel_data_end - mlmodel_data_begin);
                if (expected_size != mlmodel_data_size)
                {
                    std::ostringstream buffer;
                    buffer << service_name << ": tensor `" << *name_
                           << "` was expected to have byte size " << expected_size << " but "
                           << mlmodel_data_size << " bytes were provided";
                    throw std::invalid_argument(buffer.str());
                }
                return TfLiteTensorCopyFromBuffer(tflite_tensor_, mlmodel_data_begin, expected_size);
            }

        private:
            const std::string *name_;
            TfLiteTensor *tflite_tensor_;
        };

        // Creates a tensor_view which views a tflite tensor buffer. It dispatches on the
        // type and delegates to the templated version below.
        MLModelService::tensor_views make_tensor_view_(const MLModelService::tensor_info &info,
                                                       const TfLiteTensor *const tflite_tensor)
        {
            const auto tflite_tensor_type = TfLiteTensorType(tflite_tensor);
            switch (tflite_tensor_type)
            {
            case kTfLiteInt8:
            {
                return make_tensor_view_t_<std::int8_t>(info, tflite_tensor);
            }
            case kTfLiteUInt8:
            {
                return make_tensor_view_t_<std::uint8_t>(info, tflite_tensor);
            }
            case kTfLiteInt16:
            {
                return make_tensor_view_t_<std::int16_t>(info, tflite_tensor);
            }
            case kTfLiteUInt16:
            {
                return make_tensor_view_t_<std::uint16_t>(info, tflite_tensor);
            }
            case kTfLiteInt32:
            {
                return make_tensor_view_t_<std::int32_t>(info, tflite_tensor);
            }
            case kTfLiteUInt32:
            {
                return make_tensor_view_t_<std::uint32_t>(info, tflite_tensor);
            }
            case kTfLiteInt64:
            {
                return make_tensor_view_t_<std::int64_t>(info, tflite_tensor);
            }
            case kTfLiteUInt64:
            {
                return make_tensor_view_t_<std::uint64_t>(info, tflite_tensor);
            }
            case kTfLiteFloat32:
            {
                return make_tensor_view_t_<float>(info, tflite_tensor);
            }
            case kTfLiteFloat64:
            {
                return make_tensor_view_t_<double>(info, tflite_tensor);
            }
            default:
            {
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
        MLModelService::tensor_views make_tensor_view_t_(const MLModelService::tensor_info &info,
                                                         const TfLiteTensor *const tflite_tensor)
        {
            const auto *const tensor_data = reinterpret_cast<const T *>(TfLiteTensorData(tflite_tensor));
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
            for (const auto s : info.shape)
            {
                shape.push_back(static_cast<std::size_t>(s));
            }
            return MLModelService::make_tensor_view(tensor_data, tensor_size_t, std::move(shape));
        }

        // Accesss to the module state is serialized. All configuration
        // state is held in the `state` type to make it easier to destroy
        // the current state and replace it with a new one.
        std::shared_mutex state_rwmutex_;

        // In C++17, this could be `std::optional`.
        std::unique_ptr<struct state_> state_;
    };

} // namespace
