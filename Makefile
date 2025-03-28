.PHONY: tflite_cpu
tflite_cpu: build/Release/tflite_cpu

build/Release/tflite_cpu: src/*
	rm -rf build-conan/ && \
	conan install . --output-folder=build-conan --build=missing -o "&:shared=False" && \
	conan build . -o "&:shared=False"

.PHONY: setup
setup:
	./bin/setup.sh

module.tar.gz: build/Release/tflite_cpu
	tar -czvf module.tar.gz build/Release/tflite_cpu

.PHONY: lint
lint:
	./bin/run-clang-format.sh
