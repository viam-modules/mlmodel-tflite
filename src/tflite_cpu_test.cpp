#define BOOST_TEST_MODULE test module tflite_cpu_test

#include "tflite_cpu.hpp"

#include <boost/test/unit_test.hpp>
#include <stdexcept>

// TODO figure out how to run these tests - look at makefile exmaples in other repos

namespace vsdk = ::viam::sdk;

BOOST_AUTO_TEST_SUITE(tflite_cpu_tests)

BOOST_AUTO_TEST_CASE(configure_)
{
    // tests that empty dependencies/config variables result in an error
    /* BOOST_CHECK_EXCEPTION(configure_(vsdk::Dependencies{}, {}),
                          std::invalid_argument,
                          [](const std::invalid_argument &ex)
                          {
                              return std::string(ex.what()).find("model_path") != std::string::npos;
                          }); */

    // correct config
    BOOST_CHECK_NO_THROW(new MLModelServiceTFLite(vsdk::Dependencies{}, {{"model_path", "/path/to/test_files/model.tflite"}}));

    // incorrect path in config
    BOOST_CHECK_THROW(new MLModelServiceTFLite(vsdk::Dependencies{}, {{"model_path", "/path/to/test_files/model.onnx"}}), std::invalid_argument);
}

// BOOST_AUTO_TEST_CASE(MLModelServiceTFLite)
// {
//     // empty config
//     BOOST_CHECK_THROW(MLModelServiceTFLite(vsdk::Dependencies{}, {}), invalid_argument);

//     // test CPU detector
//     MLModelServiceTFLite tflite = MLModelServiceTFLite(vsdk::Dependencies{}, {{"model_path", "vision/tflite/effdet0.tflite"}, {"num_threads", 2}});
//     MlModelService service = tflite.MlModelService;
//     // test that the model is not nil
//     BOOST_CHECK_NE(service, null);
//     // check that config is not null
//     BOOST_CHECK_NE(tflite.state_.configuration, null);
//     // check that metadata is not null
//     BOOST_CHECK_NE(tflite.state_.metadata, null);
//     BOOST_CHECK_NE(tflite.metadata(), null);
//     // metadata tests
//     MLModelService::metadata metadata = tflite.metdata();
//     BOOST_CHECK_EQUAL(metadata.Inputs[0].Name, "images");
//     BOOST_CHECK_EQUAL(metadata.Outputs[0].Name, "location");
//     BOOST_CHECK_EQUAL(metadata.Outputs[1].Name, "category");
//     BOOST_CHECK_EQUAL(metadata.Outputs[2].Name, "score");
//     BOOST_CHECK_EQUAL(metadata.Inputs[0].DataType, "uint8");
//     BOOST_CHECK_EQUAL(metadata.Outputs[0].DataType, "float32");
//     BOOST_CHECK_EQUAL(metadata.Outputs[1].AssociatedFiles[0].Name, "labelmap.txt");

//     // TODO use openCV imread or something similar
//     Mat img = imread("vision/tflite/dogscute.jpeg")
//     // resize??
//     // convert image to bytes
// }

BOOST_AUTO_TEST_SUITE_END()