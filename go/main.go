package main

import (
	"go.viam.com/rdk/module"
	"go.viam.com/rdk/resource"
	"go.viam.com/rdk/services/mlmodel"

	"mlmodel-tflite/tflitecpu"
)

func main() {
	module.ModularMain(
		resource.APIModel{API: mlmodel.API, Model: tflitecpu.SModel},
	)
}
