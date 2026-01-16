#include <shared_mutex>

#include <tensorflow/lite/c/c_api.h>

#include <viam/sdk/resource/reconfigurable.hpp>
#include <viam/sdk/resource/stoppable.hpp>
#include <viam/sdk/services/mlmodel.hpp>

namespace mlmodel_tflite {

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
class MLModelServiceTFLite final : public viam::sdk::MLModelService,
                                   public viam::sdk::Stoppable,
                                   public viam::sdk::Reconfigurable {
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

    // Accesss to the module state is serialized. All configuration
    // state is held in the `state` type to make it easier to destroy
    // the current state and replace it with a new one.
    std::shared_mutex state_rwmutex_;

    // In C++17, this could be `std::optional`.
    std::unique_ptr<struct state_> state_;
};

}  // namespace mlmodel_tflite
