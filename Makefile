ifeq ($(OS),Windows_NT)
  BIN_EXT := .exe
  SCRIPT_EXT := .ps1
  SUBSHELL := powershell -ExecutionPolicy Bypass -File
else
  BIN_EXT :=
  SCRIPT_EXT := .sh
  SUBSHELL :=
endif
BIN := build-conan/build/Release/tflite_cpu$(BIN_EXT)

.PHONY: tflite_cpu
tflite_cpu: $(BIN)

$(BIN): src/*
	$(SUBSHELL) ./bin/build$(SCRIPT_EXT)

.PHONY: setup
setup:
	$(SUBSHELL) ./bin/setup$(SCRIPT_EXT)

module.tar.gz: tflite_cpu
ifeq ($(OS),Windows_NT)
	7z a -ttar module.tar $(BIN) meta.json
	7z a -tgzip module.tar.gz module.tar
else
	tar -czvf module.tar.gz $(BIN)
endif

.PHONY: lint
lint:
	./bin/run-clang-format.sh
