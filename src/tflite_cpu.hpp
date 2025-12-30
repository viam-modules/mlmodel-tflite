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
};