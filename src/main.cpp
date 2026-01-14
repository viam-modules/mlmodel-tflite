#include <stdexcept>

#include <viam/sdk/common/instance.hpp>
#include <viam/sdk/module/service.hpp>
#include <viam/sdk/registry/registry.hpp>
// TODO import tflite_cpu library
#include "tflite_cpu.hpp"

namespace {

int serve(const std::string& socket_path) try {
    // Every Viam C++ SDK program must have one and only one Instance object which is created before
    // any other C++ SDK objects and stays alive until all Viam C++ SDK objects are destroyed.
    viam::sdk::Instance inst;

    // Create a new model registration for the service.
    auto module_registration = std::make_shared<viam::sdk::ModelRegistration>(
        // Identify that this resource offers the MLModelService API
        viam::sdk::API::get<viam::sdk::MLModelService>(),

        // Declare a model triple for this service.
        viam::sdk::Model{"viam", "mlmodel-tflite", "tflite_cpu"},

        // Define the factory for instances of the resource.
        [](viam::sdk::Dependencies deps, viam::sdk::ResourceConfig config) {
            return std::make_shared<mlmodel_tflite::MLModelServiceTFLite>(std::move(deps),
                                                                          std::move(config));
        });

    // Register the newly created registration with the Registry.
    viam::sdk::Registry::get().register_model(module_registration);

    // Construct the module service and tell it where to place the socket path.
    auto module_service = std::make_shared<viam::sdk::ModuleService>(socket_path);

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
