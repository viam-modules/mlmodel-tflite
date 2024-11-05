# [`tflite_cpu` module](https://app.viam.com/module/viam/tflite_cpu)

This [module](https://docs.viam.com/registry/modular-resources/) implements the [`rdk:service:mlmodel` API](https://docs.viam.com/appendix/apis/services/ml/) in an `tflite_cpu` model.
With this model, you can deploy [TensorFlow Lite](https://www.tensorflow.org/lite) model files as long as your models adhere to the [model requirements](#model-requirements).
<!-- It is supported on any CPU and Linux, Raspbian, MacOS and Android machines. -->
It is supported on any CPU and Linux and MacOS machines.

## Requirements

To work with the `tflite_cpu` ML model service, an ML model is comprised of a .tflite model file which defines the model, and optionally a .txt labels file which provides the text labels for your model.
With the `tflite_cpu` ML model service, you can deploy:

- a [model from the registry](https://app.viam.com/registry)
- a [model](https://docs.viam.com/registry/ml-models/) trained outside the Viam platform that you have uploaded
- a model available on your machine

## Configure your tflite_cpu ML model

Navigate to the [**CONFIGURE** tab](https://docs.viam.com/configure/) of your [machine](https://docs.viam.com/fleet/machines/) in the [Viam app](https://app.viam.com/).
[Add `ML model / TFLite CPU` to your machine](https://docs.viam.com/configure/#services).
Enter a name or use the suggested name for your service.
Click **Add module** and click **Create**.

If you choose to use **Builder** mode in the Viam app you can choose to configure your service with an existing model on the machine or deploy a model onto your machine:

### Deploy model on machine

1. To configure your service and deploy a model onto your machine, select **Deploy model on machine** for the **Deployment** field in the resulting ML model service configuration pane.
2. Click **Select model**.
   In the modal that appears, search for models from your organization or the [Registry](/registry/).
   You can select a model to see more details about it, and then select the model to deploy it to your machine.
3. Also, optionally select the **Number of threads**.
4. Click **Save** at the top right of the window to save your changes.

### Path to Existing Model On Machine

1. To configure your service with an existing model on the machine, select **Path to existing model on machine** for the **Deployment** field.
2. Then specify the absolute **Model path** and any **Optional settings** such as the absolute **Label path** and the **Number of threads**.

### JSON mode 

You can also chose to use **JSON** mode in the Viam app. Add the `tflite_cpu` ML model object to the services array in your JSON configuration:

```json {class="line-numbers linkable-line-numbers"}
"services": [
  {
    "name": "<mlmodel_name>",
    "type": "mlmodel",
    "model": "tflite_cpu",
    "attributes": {
      "model_path": "${packages.<model_name>}/<model-name>.tflite",
      "label_path": "${packages.<model_name>}/labels.txt",
      "num_threads": <number>
    }
  },
  ... // Other services
]
```

#### Example configuration

```json {class="line-numbers linkable-line-numbers"}
{
"packages": [
  {
    "package": "39c34811-9999-4fff-bd91-26a0e4e90644/my_fruit_model",
    "version": "YYYY-MM-DDThh-mm-ss",
    "name": "my_fruit_model",
    "type": "ml_model"
  }
], ... // < Insert "components", "modules" etc. >
"services": [
  {
    "name": "fruit_classifier",
    "type": "mlmodel",
    "model": "tflite_cpu",
    "attributes": {
      "model_path": "${packages.my_fruit_model}/my_fruit_model.tflite",
      "label_path": "${packages.my_fruit_model}/labels.txt",
      "num_threads": 1
    }
  }
]
}
```

The `"packages"` array shown above is automatically created when you deploy the model.
You do not need to edit the configuration yourself, except if you wish to specify a specific [version for your deployed model](https://docs.viam.com/registry/ml-models/#versions).

The following parameters are available for a `"tflite_cpu"` model:

| Parameter | Required? | Description |
| --------- | --------- | ----------- |
| `model_path` | **Required** | The absolute path to the `.tflite model` file, as a `string`. |
| `label_path` | Optional | The absolute path to a `.txt` file that holds class labels for your TFLite model, as a `string`. This text file should contain an ordered listing of class labels. Without this file, classes will read as "1", "2", and so on. |
| `num_threads` | Optional | An integer that defines how many CPU threads to use to run inference. Default: `1`. |


If you **Deploy model on machine**, `model_path` and `label_path` will be automatically configured in the format `"${packages.<model_name>}/<model-name>.tflite"` and `"${packages.<model_name>}/labels.txt"` respectively.
If you take the **Path to existing model on machine** approach, your model and label paths do not have to be in the same format.
For example, they might resemble `home/models/fruit/my_fruit_model.tflite`.

Save the configuration.

## Model requirements

Models [trained](https://docs.viam.com/how-tos/train-deploy-ml/) in the Viam app meet these requirements by design.

We strongly recommend that you package your TensorFlow Lite model with metadata in [the standard form](https://github.com/tensorflow/tflite-support/blob/560bc055c2f11772f803916cb9ca23236a80bf9d/tensorflow_lite_support/metadata/metadata_schema.fbs).

In the absence of metadata, your `tflite_cpu` model must satisfy the following requirements:

- A single input tensor representing the image of type UInt8 (expecting values from 0 to 255) or Float 32 (values from -1 to 1).
- At least 3 output tensors (the rest won’t be read) containing the bounding boxes, class labels, and confidence scores (in that order).
- Bounding box output tensor must be ordered [x x y y], where x is an x-boundary (xmin or xmax) of the bounding box and the same is true for y.
  Each value should be between 0 and 1, designating the percentage of the image at which the boundary can be found.

These requirements are satisfied by a few publicly available model architectures including EfficientDet, MobileNet, and SSD MobileNet V1.
You can use one of these architectures or build your own.

## Next steps

The ML model service only runs your model on the machine.
To use the inferences from the model, you must use an additional service such as a [vision service](/services/vision/).

For example:

- [create a visual detector or classifier](https://docs.viam.com/services/vision/mlmodel/)
- [detect people](https://docs.viam.com/how-tos/detect-people/)

