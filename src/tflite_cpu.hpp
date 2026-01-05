#pragma once

#include <memory>
#include <viam/sdk/config/resource.hpp>
#include <viam/sdk/resource/resource.hpp>

class MLModelServiceTFLite
{

public:
    explicit MLModelServiceTFLite(viam::sdk::Dependencies dependencies,
                                  viam::sdk::ResourceConfig configuration);

    static std::unique_ptr<struct state_> configure_(viam::sdk::Dependencies dependencies,
                                                     viam::sdk::ResourceConfig configuration);

    ~MLModelServiceTFLite() final;

void stop(const vsdk::ProtoStruct &extra) noexcept final

    void stop() noexcept

    void reconfigure(const vsdk::Dependencies &dependencies,
                     const vsdk::ResourceConfig &configuration) final

    std::shared_ptr<named_tensor_views> infer(const named_tensor_views &inputs,
                                              const vsdk::ProtoStruct &extra) final

    struct metadata metadata(const vsdk::ProtoStruct &extra) final

    private :

    void check_stopped_inlock_() const

    static std::unique_ptr<struct state_> configure_(vsdk::Dependencies dependencies,
                                                     vsdk::ResourceConfig configuration)

    // Converts from tflites type enumeration into the model service
    // type enumeration or throws if there is no such conversion.
    static MLModelService::tensor_info::data_types service_data_type_from_tflite_data_type_(
        TfLiteType type)

    // All of the meaningful internal state of the service is held in
    // a separate state object to help ensure clean replacement of our
    // internals during reconfiguration.
    struct state_ final : public tflite::ErrorReporter;

    // A visitor that can populate a TFLiteTensor given a MLModelService::tensor_view.
class write_to_tflite_tensor_visitor_ : public boost::static_visitor<TfLiteStatus> public :

    TfLiteStatus
    operator()(const T &mlmodel_tensor) const
    // TODO not sure how internal classes work with header files

    // Creates a tensor_view which views a tflite tensor buffer. It dispatches on the
    // type and delegates to the templated version below.
    MLModelService::tensor_views make_tensor_view_(const MLModelService::tensor_info &info,
                                                   const TfLiteTensor *const tflite_tensor);

    // The type specific version of the above function, it just
    // reinterpret_casts the tensor buffer into an MLModelService
    // tensor view and applies the necessary shape info.
    template <typename T>
    MLModelService::tensor_views make_tensor_view_t_(const MLModelService::tensor_info &info,
                                                     const TfLiteTensor *const tflite_tensor);

    // Accesss to the module state is serialized. All configuration
    // state is held in the `state` type to make it easier to destroy
    // the current state and replace it with a new one.
    std::shared_mutex state_rwmutex_;
};