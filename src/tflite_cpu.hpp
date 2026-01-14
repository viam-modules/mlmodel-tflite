#include <shared_mutex>

#include <tensorflow/lite/c/c_api.h>

#include <viam/sdk/resource/reconfigurable.hpp>
#include <viam/sdk/resource/stoppable.hpp>
#include <viam/sdk/services/mlmodel.hpp>

namespace mlmodel_tflite {

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
class MLModelServiceTFLite : public viam::sdk::MLModelService,
                             public viam::sdk::Stoppable,
                             public viam::sdk::Reconfigurable {
    class write_to_tflite_tensor_visitor_;

   public:
    MLModelServiceTFLite(viam::sdk::Dependencies dependencies,
                         viam::sdk::ResourceConfig configuration);

    ~MLModelServiceTFLite() final;

    void stop(const viam::sdk::ProtoStruct& extra) noexcept final;

    /// @brief Stops the MLModelServiceTFLite from running.
    void stop() noexcept;

    void reconfigure(const viam::sdk::Dependencies& dependencies,
                     const viam::sdk::ResourceConfig& configuration) final;

    std::shared_ptr<named_tensor_views> infer(const named_tensor_views& inputs,
                                              const viam::sdk::ProtoStruct& extra) final;

    struct metadata metadata(const ::viam::sdk::ProtoStruct& extra) final;

   private:
    struct state_;

    void check_stopped_inlock_() const;

    static std::unique_ptr<struct state_> configure_(viam::sdk::Dependencies dependencies,
                                                     viam::sdk::ResourceConfig configuration);

    static MLModelService::tensor_info::data_types service_data_type_from_tflite_data_type_(
        TfLiteType type);

    // A visitor that can populate a TFLiteTensor given a MLModelService::tensor_view.
    class write_to_tflite_tensor_visitor_ : public boost::static_visitor<TfLiteStatus> {
       public:
        write_to_tflite_tensor_visitor_(const std::string* name, TfLiteTensor* tflite_tensor);

        template <typename T>
        TfLiteStatus operator()(const T& mlmodel_tensor) const;

       private:
        const std::string* name_;
        TfLiteTensor* tflite_tensor_;
    };

    MLModelService::tensor_views make_tensor_view_(const MLModelService::tensor_info& info,
                                                   const TfLiteTensor* const tflite_tensor);

    template <typename T>
    MLModelService::tensor_views make_tensor_view_t_(const MLModelService::tensor_info& info,
                                                     const TfLiteTensor* const tflite_tensor);

    // Accesss to the module state is serialized. All configuration
    // state is held in the `state` type to make it easier to destroy
    // the current state and replace it with a new one.
    std::shared_mutex state_rwmutex_;

    // In C++17, this could be `std::optional`.
    std::unique_ptr<struct state_> state_;
};

}  // namespace mlmodel_tflite
