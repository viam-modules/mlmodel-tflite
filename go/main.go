package main

import (
	"context"
	"mlmodel-tflite/tflitecpu"

	"go.viam.com/rdk/logging"
	"go.viam.com/rdk/module"
	"go.viam.com/rdk/services/mlmodel"
	"go.viam.com/utils"
)

func main() {
	utils.ContextualMain(mainWithArgs, module.NewLoggerFromArgs("Tflite CPU Module"))
}

func mainWithArgs(ctx context.Context, args []string, logger logging.Logger) (err error) {
	// instantiates the module itself
	m, err := module.NewModuleFromArgs(ctx, logger)
	if err != nil {
		return err
	}

	if err = m.AddModelFromRegistry(ctx, mlmodel.API, tflitecpu.SModel); err != nil {
		return err
	}

	// Each module runs as its own process
	err = m.Start(ctx)
	defer m.Close(ctx)
	if err != nil {
		return err
	}
	<-ctx.Done()
	return nil

}
