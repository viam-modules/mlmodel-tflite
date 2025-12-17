

class MLModelServiceTFLite
{

public:
    explicit MLModelServiceTFLite(vsdk::Dependencies dependencies,
                                  vsdk::ResourceConfig configuration);

    static std::unique_ptr<struct state_> configure_(vsdk::Dependencies dependencies,
                                                     vsdk::ResourceConfig configuration);
};