ifeq ($(OS),Windows_NT)
  BIN_EXT := .exe
  SCRIPT_EXT := .bat
  PATHSEP := \\
  SUBSHELL := cmd /C
else
  BIN_EXT :=
  SCRIPT_EXT := .sh
  PATHSEP := /
  SUBSHELL :=
endif

BIN := build-conan/build/Release/tflite_cpu$(BIN_EXT)

.PHONY: tflite_cpu
tflite_cpu: $(BIN)

$(BIN): conanfile.py src/*
	$(SUBSHELL) bin$(PATHSEP)build$(SCRIPT_EXT)

.PHONY: setup
setup:
	$(SUBSHELL) bin$(PATHSEP)setup$(SCRIPT_EXT)

module.tar.gz: $(BIN) meta.json
	$(SUBSHELL) bin$(PATHSEP)package$(SCRIPT_EXT) $(BIN) meta.json

.PHONY: lint
lint:
	./bin/run-clang-format.sh
