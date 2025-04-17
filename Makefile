ifeq ($(OS),Windows_NT)
  BIN_EXT := .exe
  SCRIPT_EXT := .bat
  PATHSEP := \\
  # Scripts for windows are written in powershell
  # (e.g. setup.ps1). However, I was unable to find a way to invoke
  # those scripts from here in make in a way where the exit status was
  # returned in such a way that a failed script would terminate
  # make. Instead, we invoke a little wrapper batch script with cmd
  # syntax, which calls powershell for us and then explicitly sets the
  # exit status.
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
	$(SUBSHELL) bin$(PATHSEP)package$(SCRIPT_EXT) $^

.PHONY: lint
lint:
	./bin/run-clang-format.sh
